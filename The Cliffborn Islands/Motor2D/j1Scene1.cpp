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
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"

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

		debug_tex = App->tex->Load("maps/path2.png");

		// The audio is played
		App->audio->PlayMusic("audio/music/level1_music.ogg", 1.0f);

		if (!player_created)
		{
			App->entity->CreatePlayer();
			player_created = true;
		}

		PlaceEnemies();
	}

	return true;
}

// Called each loop iteration
bool j1Scene1::PreUpdate()
{
	BROFILER_CATEGORY("Level1PreUpdate", Profiler::Color::Orange)

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
bool j1Scene1::Update(float dt)
{
	BROFILER_CATEGORY("Level1Update", Profiler::Color::LightSeaGreen)

	// Load and Save
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->entity->DestroyEntities();
		App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) 
	{
		App->fade->FadeToBlack(App->scene1, App->scene1);
		App->entity->player->position = initialScene1Position;
		App->render->camera.x = 0;
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		ChangeScene();		

	App->map->Draw();

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
bool j1Scene1::PostUpdate()
{
	BROFILER_CATEGORY("Level1PostUpdate", Profiler::Color::Yellow)

	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
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

void j1Scene1::PlaceEnemies()
{
	/*App->entity->AddEnemy(210, 120, SKELETON);
	App->entity->AddEnemy(613, 73, SKELETON);
	App->entity->AddEnemy(1676, 9, SKELETON);
	App->entity->AddEnemy(2146, 104, SKELETON);

	App->entity->AddEnemy(250, 50, HARPY);
	App->entity->AddEnemy(400, 20, HARPY);
	App->entity->AddEnemy(600, 20, HARPY);
	App->entity->AddEnemy(917, 13, HARPY);
	App->entity->AddEnemy(1079, 17, HARPY);
	App->entity->AddEnemy(1345, 8, HARPY);
	App->entity->AddEnemy(1772, 73, HARPY);
	App->entity->AddEnemy(1919, 73, HARPY);
	App->entity->AddEnemy(2193, 33, HARPY);
	App->entity->AddEnemy(2355, 32, HARPY);*/
}

// Called before quitting
bool j1Scene1::CleanUp()
{
	LOG("Freeing scene");

	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->entity->DestroyEntities();
	if (App->entity->player)
		App->entity->player->CleanUp();
	App->path->CleanUp();

	return true;
}

void j1Scene1::ChangeScene()
{
	App->scene2->active = true;
	App->scene1->active = false;
	CleanUp();
	App->fade->FadeToBlack(App->scene1, App->scene2);
	App->entity->Start();
	App->scene2->Start();
	App->render->camera = { 0,0 };
	App->path->Start();
	App->entity->player->position = initialScene1Position;
}
