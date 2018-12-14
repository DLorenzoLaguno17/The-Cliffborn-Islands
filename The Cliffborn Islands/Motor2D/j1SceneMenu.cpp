#include "j1SceneMenu.h"
#include "j1SceneCredits.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1EntityManager.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "j1Gui.h"
#include "j1Button.h"
#include "j1Label.h"
#include "j1Box.h"
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
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creating UI		
		SDL_Rect section = { 537, 0, 663, 712 };
		settings_window = App->gui->CreateBox(BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, section, gui_tex);

		SDL_Rect idle = {0, 143, 190, 49};
		SDL_Rect hovered = { 0, 45, 190, 49 };
		SDL_Rect clicked = { 0, 94, 190, 49 };

		App->gui->CreateButton(&menuButtons, BUTTON, 80, 110, idle, hovered, clicked, gui_tex, PLAY_GAME);
		App->gui->CreateButton(&menuButtons, BUTTON, 80, 135, idle, hovered, clicked, gui_tex, CONTINUE);
		App->gui->CreateButton(&menuButtons, BUTTON, 80, 160, idle, hovered, clicked, gui_tex, OPEN_CREDITS);

		SDL_Rect idle2 = { 28, 201, 49, 49 };
		SDL_Rect hovered2 = { 77, 201, 49, 49 };
		SDL_Rect clicked2 = { 126, 201, 49, 49 };
		App->gui->CreateButton(&menuButtons, BUTTON, 228, 3, idle2, hovered2, clicked2, gui_tex, CLOSE_GAME);
		App->gui->CreateButton(&menuButtons, BUTTON, 20, 20, idle2, hovered2, clicked2, gui_tex, CLOSE_GAME, (j1UserInterfaceElement*)settings_window);

		SDL_Rect idle3 = { 463, 109, 49, 49 };
		SDL_Rect hovered3 = { 463, 158, 49, 49 };
		SDL_Rect clicked3 = { 463, 207, 49, 49 };
		App->gui->CreateButton(&menuButtons, BUTTON, 3, 3, idle3, hovered3, clicked3, gui_tex, SETTINGS);

		App->gui->CreateLabel(&menuLabels, LABEL, 106, 115, font, "Start", { 245, 245, 220, 255 });
		App->gui->CreateLabel(&menuLabels, LABEL, 90, 140, font, "Continue", { 245, 245, 220, 255 });
		App->gui->CreateLabel(&menuLabels, LABEL, 98, 165, font, "Credits", { 245, 245, 220, 255 });
		App->gui->CreateLabel(&menuLabels, LABEL, 44, 9, font, "Settings", { 73, 31, 10, 255 }, (j1UserInterfaceElement*)settings_window);

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

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------	

	App->gui->UpdateButtonsState(&menuButtons); 
	App->gui->UpdateBoxesState();
	
	// Button actions
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
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
				startGame = true;
				App->fade->FadeToBlack();
			}
			else if (item->data->bfunction == CONTINUE) {
				loadGame = true;
				App->fade->FadeToBlack();
			}
			else if (item->data->bfunction == CLOSE_GAME) {
				continueGame = false;
			}
			else if (item->data->bfunction == SETTINGS) {
				settings_window->visible = !settings_window->visible;
				settings_window->position = App->gui->settingsPosition;
			}
			else if (item->data->bfunction == OPEN_CREDITS) {
				openCredits = true;
				App->fade->FadeToBlack();
			}
			break;

		case CLICKED:
			item->data->situation = item->data->clicked;
			break;
		}
	}

	// Managing scene transitions
	if (App->fade->IsFading() == 0) {
		if (startGame) {
			ChangeScene(SCENE1);
			player_created = true;
		}
		else if (openCredits)
			ChangeScene(CREDITS);
		else if (loadGame)
			App->LoadGame("save_game.xml");
	}

	// To move settings window in case it is visible
	if (settings_window != nullptr) {
		if (settings_window->clicked) {
			int x, y; App->input->GetMousePosition(x, y);

			if (settings_window->distanceCalculated == false) {
				settings_window->mouseDistance = { x - settings_window->position.x, y - settings_window->position.y };
				settings_window->distanceCalculated = true;
			}

			settings_window->position = { x - settings_window->mouseDistance.x, y - settings_window->mouseDistance.y };

			for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
				if (item->data->parent == settings_window) {
					item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
					item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
				}
			}

			for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
				if (item->data->parent == settings_window) {
					item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
					item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
				}
			}
		}
		else
			settings_window->distanceCalculated = false;
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	// Blitting background and animations
	App->map->Draw();

	SDL_Rect p = player.GetCurrentFrame(dt);
	App->render->Blit(player_tex, 40, 105, &p, SDL_FLIP_NONE);
	SDL_Rect h = harpy.GetCurrentFrame(dt);
	App->render->Blit(harpy_tex, 205, 35, &h, SDL_FLIP_HORIZONTAL);

	// Blitting the buttons and labels of the menu
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		item->data->Draw(App->gui->buttonsScale);
	}
	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		item->data->Draw();
	}

	// Bliting the logo
	SDL_Rect logo = { 166, 139, 711, 533 };
	App->render->Blit(logo_tex, 54, 0, &logo, SDL_FLIP_NONE, 1.0f, App->gui->logoScale);

	// Blitting settings window
	if (settings_window != nullptr && settings_window->visible == true)
		settings_window->Draw(App->gui->boxScale);

	// Blitting the buttons and labels of the window
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;
		item->data->Draw(App->gui->buttonsScale);
	}
	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;
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
	App->tex->UnLoad(logo_tex);
	App->tex->UnLoad(harpy_tex);
	App->tex->UnLoad(player_tex);
	App->tex->UnLoad(gui_tex);
	
	App->map->CleanUp();
	App->tex->CleanUp();

	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		menuButtons.del(item);
	}

	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		menuLabels.del(item);
	}

	delete settings_window;
	if(settings_window != nullptr) settings_window = nullptr;

	return true;
}

bool j1SceneMenu::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	if ((scene_activated == false) && active)
		ChangeScene(SCENE1);
	else
		ChangeScene(SCENE2);

	return true;
}

void j1SceneMenu::ChangeScene(SCENE objectiveScene)
{
	if (!player_created)
	{
		this->active = false;
		startGame = false;
		loadGame = false;
		openCredits = false;

		CleanUp();

		if (objectiveScene == SCENE::CREDITS) {
			App->credits->active = true;
			App->credits->Start();
		}
		else {
			if (objectiveScene == SCENE::SCENE1) {
				App->scene1->active = true;
				App->scene1->Start();
			}
			else if (objectiveScene == SCENE::SCENE2) {
				App->scene2->active = true;
				App->scene2->Start();
			}

			App->entity->active = true;
			App->entity->CreatePlayer();
			App->entity->Start();
		}
	}
}