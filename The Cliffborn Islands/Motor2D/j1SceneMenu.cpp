#include "j1SceneMenu.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Scene1.h"
#include "j1EntityManager.h"
#include "j1Collisions.h"
#include "j1Pathfinding.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "j1Gui.h"
#include "j1Button.h"
#include "j1Label.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Window.h"

#include "Brofiler/Brofiler.h"

j1SceneMenu::j1SceneMenu()
{
	name.create("menu");

	player.LoadAnimations("idle");
	harpy.LoadEnemyAnimations("idle", "harpy");
}

j1SceneMenu::~j1SceneMenu() {}

bool j1SceneMenu::Awake(pugi::xml_node &)
{
	LOG("Loading Menu");
	bool ret = true;

	if (App->menu->active == false)
	{
		LOG("Unable to load the menu.");
		ret = false;
	}

	return ret;
}

bool j1SceneMenu::Start()
{	
	if (active)
	{
		// The map is loaded
		if (App->map->Load("menu.tmx"))
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
		App->audio->PlayMusic("audio/music/menu_music.ogg", 1.0f);

		// Loading textures
		gui_tex = App->tex->Load("gui/atlas.png");
		player_tex = App->tex->Load("textures/character/character.png");
		harpy_tex = App->tex->Load("textures/enemies/harpy/harpy.png");
		
		// Loading fonts
		text = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		SDL_Rect idle = {0, 143, 190, 49};
		SDL_Rect hovered = { 0, 45, 190, 49 };
		SDL_Rect clicked = { 0, 94, 190, 49 };

		App->gui->CreateButton(BUTTON, 85, 110, idle, hovered, clicked, gui_tex, PLAY_GAME);
		App->gui->CreateButton(BUTTON, 85, 135, idle, hovered, clicked, gui_tex, CLOSE_GAME);
		App->gui->CreateButton(BUTTON, 85, 160, idle, hovered, clicked, gui_tex, CREDITS);

		App->gui->CreateLabel(LABEL, 111, 115, text, "Start", { 245, 245, 220, 255 });
		App->gui->CreateLabel(LABEL, 116, 140, text, "Exit", { 245, 245, 220, 255 });
		App->gui->CreateLabel(LABEL, 103, 165, text, "Credits", { 245, 245, 220, 255 });
	}

	return true;
}

bool j1SceneMenu::PreUpdate()
{
	BROFILER_CATEGORY("MenuPreUpdate", Profiler::Color::Orange)
	return true;
}

bool j1SceneMenu::Update(float dt)
{
	BROFILER_CATEGORY("MenuUpdate", Profiler::Color::LightSeaGreen)

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		App->audio->MusicVolume(App->audio->GetMusicVolume() + 10.0f);

	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
		App->audio->MusicVolume(App->audio->GetMusicVolume() - 10.0f);

	App->gui->UpdateButtonsState();
	
	// UI management
	for (p2List_item<j1Button*>* item = App->gui->buttons.start; item != nullptr; item = item->next) {
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
			if (item->data->bfunction == PLAY_GAME) {
				ChangeScene();
			}
			else if (item->data->bfunction == CLOSE_GAME) {
				continueGame = false;				
			}
			else if (item->data->bfunction == CREDITS){
				ShellExecuteA(NULL, "open", "https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands", NULL, NULL, SW_SHOWNORMAL);
			}
			break;

		case CLICKED:
			item->data->situation = item->data->clicked;
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	App->map->Draw();

	SDL_Rect p = player.GetCurrentFrame(dt);
	App->render->Blit(player_tex, 40, 105, &p, SDL_FLIP_NONE);
	SDL_Rect h = harpy.GetCurrentFrame(dt);
	App->render->Blit(harpy_tex, 200, 25, &h, SDL_FLIP_HORIZONTAL);

	// Blitting the buttons
	for (p2List_item<j1Button*>* item = App->gui->buttons.start; item != nullptr; item = item->next) {
		item->data->Draw(0.5f);
	}

	// Blitting the labels
	for (p2List_item<j1Label*>* item = App->gui->labels.start; item != nullptr; item = item->next) {
		item->data->Draw();
	}

	return true;
}

bool j1SceneMenu::PostUpdate()
{
	BROFILER_CATEGORY("MenuPostUpdate", Profiler::Color::Yellow)

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		continueGame = false;

	return continueGame;
}

bool j1SceneMenu::CleanUp()
{
	LOG("Freeing all textures");
	App->tex->UnLoad(harpy_tex);
	App->tex->UnLoad(player_tex);
	App->tex->UnLoad(gui_tex);

	return true;
}

void j1SceneMenu::ChangeScene()
{
	this->active = false;
	App->scene1->active = true;
	CleanUp();
	App->scene1->Start();
	App->entity->active = true;
	App->entity->CreatePlayer();
	App->entity->Start();
	App->collisions->Start();
	App->path->Start();
	App->scene1->Update(0);
}
