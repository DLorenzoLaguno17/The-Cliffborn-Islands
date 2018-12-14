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
#include "j1Scene2.h"
#include "j1Scene1.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Coin.h"
#include "j1Fonts.h"
#include "j1Label.h"

#include "Brofiler/Brofiler.h"

j1Scene2::j1Scene2() : j1Module()
{
	name.create("scene2");
}

// Destructor
j1Scene2::~j1Scene2()
{}

// Called before render is available
bool j1Scene2::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene 2");
	bool ret = true;

	if (App->menu->active || App->scene1->active)
		active = false;

	if (active == false)
		LOG("Scene 2 not active");

	// Copying the position of the player
	initialScene2Position.x = config.child("initialPlayerPosition").attribute("x").as_int();
	initialScene2Position.y = config.child("initialPlayerPosition").attribute("y").as_int();

	if (App->scene1->active == true) 
		active = false; 

	return ret;
}

// Called before the first frame
bool j1Scene2::Start()
{
	if (active)
	{
		// The map is loaded
		if (App->map->Load("lvl2.tmx"))
		{
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
			{
				App->path->SetMap(w, h, data);
			}

			RELEASE_ARRAY(data);
		}
		text = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);
		debug_tex = App->tex->Load("maps/path2.png");
		coin_hud.LoadEnemyAnimations("idle", "coin");
		coin_tex = App->tex->Load("textures/coin.png");
		animation = &coin_hud;

		PlaceEntities();

		startup_time.Start();

		time_text = { "%i", time_scene2 };

		seconds = App->gui->CreateLabel(&scene2Labels, LABEL, 500, 0, text, time_text.GetString());
		minutes = App->gui->CreateLabel(&scene2Labels, LABEL, 410, 0, text, "00:");

		// The audio is played
		App->audio->PlayMusic("audio/music/level1_music.ogg", 1.0f);
	}
	
	return true;
}

// Called each loop iteration
bool j1Scene2::PreUpdate()
{
	BROFILER_CATEGORY("Level2PreUpdate", Profiler::Color::Orange)

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	if (App->collisions->debug) {
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->map->WorldToMap(p.x, p.y);

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			if (origin_selected == true)
			{
				App->path->CreatePath(origin, p);
				origin_selected = false;
			}
			else
			{
				origin = p;
				origin_selected = true;
			}
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene2::Update(float dt)
{
	BROFILER_CATEGORY("Level2Update", Profiler::Color::LightSeaGreen)
	
	time_scene2 = startup_time.ReadSec();


	// Load and Save
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->entity->DestroyEntities();
		App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || changingScene) {
		changingScene = true;

		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0)
			ChangeScene();
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN || resettingLevel)
	{
		resettingLevel = true;
		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0) {
			App->entity->player->position = initialScene2Position;
			App->render->camera.x = 0;
			App->entity->player->facingRight = true;
			resettingLevel = false;
		}
	}

	App->map->Draw();

	SDL_Rect r = animation->GetCurrentFrame(dt);
	App->render->Blit(coin_tex, 3, 700, &r, SDL_FLIP_NONE, 1.0f, 1, 0, INT_MAX, INT_MAX, false);

	if (App->collisions->debug) {
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);

		// Debug pathfinding ------------------------------
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);
		p = App->map->WorldToMap(p.x, p.y);
		p = App->map->MapToWorld(p.x, p.y);

		App->render->Blit(debug_tex, p.x, p.y);

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
bool j1Scene2::PostUpdate()
{
	BROFILER_CATEGORY("Level2PostUpdate", Profiler::Color::Yellow)
	bool ret = true;


	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	time_text = { "%i", time_scene2 };
	if (time_scene2 == 60)
	{
		min += 1;
		App->tex->UnLoad(minutes->sprites);
		startup_time.Start();
		time_text = { "%i", time_scene2 };
		if (min < 10)
		{
			min_text_left.Clear();
			min_text = { "%i", min };
			min_text_left.operator+=("0");
			min_text_left.operator+=(min_text);
			min_text_left.operator+=(":");
			minutes->sprites = App->font->Print(min_text_left.GetString(), minutes->color, minutes->font);
		}
		else
		{
			min_text = { "%i", min };
			min_text.operator+=(":");
			minutes->sprites = App->font->Print(min_text.GetString(), minutes->color, minutes->font);
		}
	}
	App->tex->UnLoad(seconds->sprites);
	seconds->sprites = App->font->Print(time_text.GetString(), seconds->color, seconds->font);

	if (seconds->sprites != nullptr)
		seconds->Draw(1.0f, 0, 0, false);
	if (minutes->sprites != nullptr)
		minutes->Draw(1.0f, 0, 0, false);
	return ret;
}

bool j1Scene2::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	if ((scene_activated == false) && active)
		ChangeScene(); 

	return true;
}

bool j1Scene2::Save(pugi::xml_node& node) const
{
	pugi::xml_node activated = node.append_child("activated");

	activated.append_attribute("true") = active;

	return true;
}


// Called before quitting
bool j1Scene2::CleanUp()
{
	LOG("Freeing scene");
	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->entity->DestroyEntities();
	App->gui->CleanUp();

	if (App->entity->player)
		App->entity->player->CleanUp();
	if (App->entity->hook)
		App->entity->hook->CleanUp();

	for (p2List_item<j1Button*>* item = scene2Buttons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene2Buttons.del(item);
	}

	for (p2List_item<j1Label*>* item = scene2Labels.start; item != nullptr; item = item->next) {
		scene2Labels.del(item);
	}

	App->path->CleanUp();

	return true;
}

void j1Scene2::PlaceEntities()
{
	p2SString folder;
	pugi::xml_document	map_file;

	p2SString tmp("%s%s", folder.GetString(), "maps/lvl2.tmx");

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

void j1Scene2::ChangeScene()
{
	App->scene1->active = true;
	App->scene2->active = false;
	CleanUp();	
	App->entity->Start();
	App->scene1->Start();
	App->render->camera = { 0,0 };
	App->path->Start();
	App->entity->player->initialPosition = App->scene1->initialScene1Position;
	App->entity->player->position = App->scene1->initialScene1Position;
	changingScene = false;
}


void j1Scene2::ChangeSceneMenu()
{
	App->scene2->active = false;
	App->menu->active = true;

	CleanUp();
	App->entity->CleanUp();
	App->entity->active = false;
	App->menu->Start();
	App->fade->FadeToBlack();
	App->render->camera = { 0,0 };
}
