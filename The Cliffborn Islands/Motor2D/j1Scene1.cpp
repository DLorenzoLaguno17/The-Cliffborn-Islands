#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collisions.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Hook.h"
#include "j1Player.h"
#include "j1SceneMenu.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1SceneMenu.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Button.h"
#include "j1Box.h"

#include "Brofiler/Brofiler.h"

j1Scene1::j1Scene1() : j1Module()
{
	name.create("scene1");
}

// Destructor
j1Scene1::~j1Scene1()
{}

// Called before render is available
bool j1Scene1::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene 1");
	bool ret = true;

	if (App->menu->active == true)
		active = false;

	if (active == false)
		LOG("Scene1 not active.");

	// Copying the position of the player
	initialScene1Position.x = config.child("initialPlayerPosition").attribute("x").as_int();
	initialScene1Position.y = config.child("initialPlayerPosition").attribute("y").as_int();

	return ret;
}

// Called before the first frame
bool j1Scene1::Start()
{
	if (App->scene2->active == true) { active = false; }

	if (active)
	{
		// The map is loaded
		if (App->map->Load("lvl1.tmx"))
		{
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
			{
				App->path->SetMap(w, h, data);
			}				

			RELEASE_ARRAY(data);
		}

		// The audio is played	
		App->audio->PlayMusic("audio/music/level1_music.ogg", 1.0f);

		// Textures are loaded
		debug_tex = App->tex->Load("maps/path2.png");
		gui_tex = App->tex->Load("gui/atlas.png");

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creating UI
		SDL_Rect section = { 537, 0, 663, 712 };
		settings_window = App->gui->CreateBox(&scene1Boxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, section, gui_tex);
		settings_window->visible = false;

		uint minimum = 58;
		uint maximum = 108;

		App->gui->CreateBox(&scene1Boxes, BOX, 83, 42, { 416, 72, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, minimum, maximum);
		App->gui->CreateBox(&scene1Boxes, BOX, 83, 82, { 416, 72, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, minimum, maximum);

		App->gui->CreateLabel(&scene1Labels, LABEL, 44, 9, font, "Settings", { 73, 31, 10, 255 }, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 30, 50, font, "Sound", { 73, 31, 10, 255 }, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 30, 89, font, "Music", { 73, 31, 10, 255 }, (j1UserInterfaceElement*)settings_window);

		PlaceEntities();

		startup_time.Start();
	}

	return true;
}

// Called each loop iteration
bool j1Scene1::PreUpdate()
{
	BROFILER_CATEGORY("Level1PreUpdate", Profiler::Color::Orange)

	return true;
}

// Called each loop iteration
bool j1Scene1::Update(float dt)
{
	BROFILER_CATEGORY("Level1Update", Profiler::Color::LightSeaGreen)

	time_scene1 = startup_time.ReadSec();

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------	

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		settings_window->visible = !settings_window->visible;
		App->gamePaused = !App->gamePaused;		

		if(App->render->camera.x != 0)
			settings_window->position = { (int)App->entity->player->position.x - App->gui->settingsPosition.x, App->gui->settingsPosition.y };
	}

	App->gui->UpdateButtonsState(&scene1Buttons);
	App->gui->UpdateWindow(settings_window, &scene1Buttons, &scene1Labels, &scene1Boxes);

	// Button actions
	for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next) {
		switch (item->data->state)
		{
		case IDLE:
			item->data->situation = item->data->idle;
			break;

		case HOVERED:
			item->data->situation = item->data->hovered;
			break;

		case RELEASED:
			item->data->situation = item->data->idle;
			if (item->data->bfunction == GO_TO_MENU) {
				backToMenu = true;
				App->fade->FadeToBlack();
			}
			/*else if (item->data->bfunction == CONTINUE) {
			loadGame = true;
			App->fade->FadeToBlack();
			}
			else if (item->data->bfunction == CLOSE_GAME) {
			continueGame = false;
			}
			else if (item->data->bfunction == SETTINGS) {
			settings_window->visible = !settings_window->visible;
			settings_window->position = settingsPosition;
			}
			else if (item->data->bfunction == OPEN_CREDITS) {
			openCredits = true;
			App->fade->FadeToBlack();
			}*/
			break;

		case CLICKED:
			item->data->situation = item->data->clicked;
			break;
		}
	}

	// Load and Save
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	// Managing scene transitions
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN || resettingLevel)
	{
		resettingLevel = true;
		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0) {
			App->entity->player->position = initialScene1Position;
			App->render->camera.x = 0;
			App->entity->player->facingRight = true;
			resettingLevel = false;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN || changingScene) {
		changingScene = true;

		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0)
			ChangeScene();
	}

	if (backToMenu && App->fade->IsFading() == 0) {
		ChangeSceneMenu();
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	App->map->Draw();

	// Blitting patfhinding if debug is activated
	if (App->collisions->debug) {
		const p2DynArray<iPoint>* path = App->path->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			App->render->Blit(debug_tex, pos.x, pos.y);
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene1::PostUpdate()
{
	BROFILER_CATEGORY("Level1PostUpdate", Profiler::Color::Yellow)

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		continueGame = false;

	return continueGame;
}

bool j1Scene1::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	if ((scene_activated == false) && active)
		ChangeScene(); 	

	return true;
}

bool j1Scene1::Save(pugi::xml_node& node) const
{
	pugi::xml_node activated = node.append_child("activated");

	activated.append_attribute("true") = active;

	return true;
}

void j1Scene1::PlaceEntities()
{
	p2SString folder;
	pugi::xml_document	map_file;

	p2SString tmp("%s%s", folder.GetString(), "maps/lvl1.tmx");

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());
	
	pugi::xml_node obj;
	pugi::xml_node group;
	
	const char* object_name;

	for (group = map_file.child("map").child("objectgroup"); group; group = group.next_sibling("objectgroup"))
	{
		object_name = group.attribute("name").as_string();

		for (obj = group.child("object"); obj; obj = obj.next_sibling("object"))
		{
			if (strcmp(object_name, "coins") == 0)
				App->entity->AddEnemy(obj.attribute("x").as_int(), obj.attribute("y").as_int(), COIN);
			else if (strcmp(object_name, "skeleton") == 0)
				App->entity->AddEnemy(obj.attribute("x").as_int(), obj.attribute("y").as_int(), SKELETON);
			else if (strcmp(object_name, "harpy") == 0)
				App->entity->AddEnemy(obj.attribute("x").as_int(), obj.attribute("y").as_int(), HARPY);
		}
	}
}

// Called before quitting
bool j1Scene1::CleanUp()
{
	LOG("Freeing scene");
	App->tex->UnLoad(gui_tex);
	App->tex->UnLoad(debug_tex);

	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->entity->DestroyEntities();
	App->gui->CleanUp();

	if (App->entity->player)
		App->entity->player->CleanUp();
	if (App->entity->hook)
		App->entity->hook->CleanUp();

	for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene1Buttons.del(item);
	}

	for (p2List_item<j1Label*>* item = scene1Labels.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene1Labels.del(item);
	}

	for (p2List_item<j1Box*>* item = scene1Boxes.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene1Boxes.del(item);
	}

	delete settings_window;
	if (settings_window != nullptr) settings_window = nullptr;

	App->path->CleanUp();

	return true;
}

void j1Scene1::ChangeScene()
{
	App->scene2->active = true;
	App->scene1->active = false;
	CleanUp();
	App->entity->Start();
	App->scene2->Start();
	App->render->camera = { 0,0 };
	App->path->Start();
	App->entity->player->initialPosition = App->scene2->initialScene2Position;
	App->entity->player->position = App->scene2->initialScene2Position;
	changingScene = false;
}

void j1Scene1::ChangeSceneMenu()
{
	App->scene1->active = false;
	App->menu->active = true;

	CleanUp();
	App->fade->FadeToBlack();
	App->entity->CleanUp();
	App->entity->active = false;
	App->menu->Start();
	App->render->camera = { 0,0 };
	backToMenu = false;
}
