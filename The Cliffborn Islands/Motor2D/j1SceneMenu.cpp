#include "j1SceneMenu.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1EntityManager.h"
#include "j1Collisions.h"
#include "j1Pathfinding.h"
#include "j1FadeToBlack.h"
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
		App->map->Load("menu.tmx");

		// The audio is played
		App->audio->PlayMusic("audio/music/menu_music.ogg", 1.0f);

		// Loading textures
		gui_tex = App->tex->Load("gui/atlas.png");
		logo_tex = App->tex->Load("gui/logo.png");
		player_tex = App->tex->Load("textures/character/character.png");
		harpy_tex = App->tex->Load("textures/enemies/harpy/harpy.png");
		
		// Loading fonts
		text = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		SDL_Rect idle = {0, 143, 190, 49};
		SDL_Rect hovered = { 0, 45, 190, 49 };
		SDL_Rect clicked = { 0, 94, 190, 49 };

		App->gui->CreateButton(BUTTON, 85, 110, idle, hovered, clicked, gui_tex, PLAY_GAME);
		App->gui->CreateButton(BUTTON, 85, 135, idle, hovered, clicked, gui_tex, CONTINUE);
		App->gui->CreateButton(BUTTON, 85, 160, idle, hovered, clicked, gui_tex, CREDITS);

		SDL_Rect idle2 = { 28, 201, 49, 49 };
		SDL_Rect hovered2 = { 77, 201, 49, 49 };
		SDL_Rect clicked2 = { 126, 201, 49, 49 };
		App->gui->CreateButton(BUTTON, 229, 3, idle2, hovered2, clicked2, gui_tex, CLOSE_GAME);

		SDL_Rect idle3 = { 463, 109, 49, 49 };
		SDL_Rect hovered3 = { 463, 158, 49, 49 };
		SDL_Rect clicked3 = { 463, 207, 49, 49 };
		App->gui->CreateButton(BUTTON, 3, 3, idle3, hovered3, clicked3, gui_tex, SETTINGS);

		App->gui->CreateLabel(LABEL, 111, 115, text, "Start", { 245, 245, 220, 255 });
		App->gui->CreateLabel(LABEL, 95, 140, text, "Continue", { 245, 245, 220, 255 });
		App->gui->CreateLabel(LABEL, 103, 165, text, "Credits", { 245, 245, 220, 255 });

		player_created = false;
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
		
	// Volume control
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
				//startGame = true;
				//App->fade->FadeToBlack();
				ChangeScene(true);
				player_created = true;
			}
			else if (item->data->bfunction == CONTINUE) {
				App->LoadGame("save_game.xml");
			}
			else if (item->data->bfunction == CLOSE_GAME) {
				continueGame = false;				
			}
			else if (item->data->bfunction == SETTINGS) {

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

	/*if (startGame) {
		if (App->fade->IsFading() == 0)
			ChangeScene(true);
	}*/

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	App->map->Draw();

	SDL_Rect p = player.GetCurrentFrame(dt);
	App->render->Blit(player_tex, 40, 105, &p, SDL_FLIP_NONE);
	SDL_Rect h = harpy.GetCurrentFrame(dt);
	App->render->Blit(harpy_tex, 205, 35, &h, SDL_FLIP_HORIZONTAL);

	// Blitting the buttons
	for (p2List_item<j1Button*>* item = App->gui->buttons.start; item != nullptr; item = item->next) {
		item->data->Draw(0.5f);
	}

	// Blitting the labels
	for (p2List_item<j1Label*>* item = App->gui->labels.start; item != nullptr; item = item->next) {
		item->data->Draw();
	}

	SDL_Rect logo = { 166, 139, 711, 533};
	App->render->Blit(logo_tex, 59, 5, &logo, SDL_FLIP_NONE, 1.0f, 0.20f);

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
	App->tex->UnLoad(logo_tex);
	App->tex->UnLoad(harpy_tex);
	App->tex->UnLoad(player_tex);
	App->tex->UnLoad(gui_tex);

	App->map->CleanUp();
	App->tex->CleanUp();

	return true;
}

bool j1SceneMenu::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	if ((scene_activated == false) && active)
		ChangeScene(scene_activated);

	return true;
}

void j1SceneMenu::ChangeScene(bool scene1)
{
	if (!player_created)
	{
		this->active = false;
		CleanUp();

		if (scene1) {
			App->scene1->active = true;
			App->scene1->Start();
		}
		else if (!scene1) {
			App->scene2->active = true;
			App->scene2->Start();
		}

		App->entity->active = true;
		App->entity->CreatePlayer();
		App->entity->Start();
		App->path->Start();
		startGame = false;
	}
}
