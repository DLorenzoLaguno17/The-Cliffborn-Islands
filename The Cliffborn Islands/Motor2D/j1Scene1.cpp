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
#include "j1Player.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1FadeToBlack.h"

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

	cameraLimit = config.child("camera").attribute("cameraLimit").as_int();
	playerLimit = config.child("camera").attribute("playerLimit").as_int();


	return ret;
}

// Called before the first frame
bool j1Scene1::Start()
{

	if (active)
	{
		// The map is loaded
		App->map->Load("lvl1.tmx");

		// The audio is played
		App->audio->PlayMusic("audio/music/level1_music.ogg", 1.0f);
	}

	return true;
}

// Called each loop iteration
bool j1Scene1::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene1::Update(float dt)
{
	// Load and Save
	if(App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	// Control of the camera
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 5;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 5;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 5;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 5;

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) 
	{
		App->player->position.x = 0;
		App->player->position.y = 20;
		App->render->camera.x = 0;
	}
		

	// Camera control
	if (App->render->camera.x > cameraLimit)
	{
		App->render->camera.x = -App->player->position.x * 4 + 400;
		if (App->render->camera.x > 0)
			App->render->camera.x = 0;
	}		
	
	// Limit player X position
	if (App->player->position.x > playerLimit)
		App->player->position.x = playerLimit;

	if (App->player->position.x < 0)
		App->player->position.x = 0;

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene1::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene1::CleanUp()
{
	LOG("Freeing scene");

	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->player->CleanUp();

	return true;
}

void j1Scene1::ChangeScene()
{
	App->scene2->active = true;
	App->scene1->active = false;
	CleanUp();
	App->player->CleanUp();
	App->player->Start();
	App->render->camera = { 0,0 };
	App->scene2->Start();
}
