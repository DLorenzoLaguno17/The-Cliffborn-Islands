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
#include "j1Input.h"
#include "j1Window.h"

#include "Brofiler/Brofiler.h"

j1SceneMenu::j1SceneMenu()
{
	name.create("menu");
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

		gui_tex = App->tex->Load("gui/atlas.png");

		SDL_Rect idle = {288, 198, 49, 49};
		SDL_Rect hovered = { 239, 198, 49, 49 };
		SDL_Rect clicked = { 190, 198, 49, 49 };

		App->gui->CreateButton(BUTTON, 170, 80, idle, hovered, clicked, gui_tex, PLAY_GAME);
		App->gui->CreateButton(BUTTON, 170, 130, idle, hovered, clicked, gui_tex, CLOSE_GAME);
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

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		ChangeScene();

	App->gui->UpdateButtonsState();
	
	// UI management
	for (p2List_item<j1Button*>* item = App->gui->buttons.start; item != nullptr; item = item->next) {
		switch (item->data->state) 
		{
		case IDLE:
			item->data->section = item->data->idle;
			break;

		case HOVERED:
			item->data->section = item->data->hovered;
			break;

		case RELEASED:
			item->data->section = item->data->idle;
			if (item->data->bfunction == PLAY_GAME) {
				ChangeScene();
			}
			else if (item->data->bfunction == CLOSE_GAME) {
				continueGame = false;
			}
			break;

		case CLICKED:
			item->data->section = item->data->clicked;
			break;
		}
	}

	App->map->Draw();

	// Blitting the buttons
	for (p2List_item<j1Button*>* item = App->gui->buttons.start; item != nullptr; item = item->next) {
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
