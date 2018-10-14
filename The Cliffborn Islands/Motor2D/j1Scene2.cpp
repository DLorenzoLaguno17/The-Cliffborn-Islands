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
#include "j1Scene2.h"
#include "j1Scene1.h"
#include "j1FadeToBlack.h"

#define CAMERA_LIMIT -8570
#define PLAYER_LIMIT 2374

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
	cameraLimit = config.child("camera").attribute("cameraLimit").as_int();
	playerLimit = config.child("camera").attribute("playerLimit").as_int();

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
		App->map->Load("lvl2.tmx"); 
		// The audio is played
		App->audio->PlayMusic("audio/music/level2_music.ogg", 1.0f); 
	}
	
	return true;
}

// Called each loop iteration
bool j1Scene2::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene2::Update(float dt)
{
	// Load and Save
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		ChangeScene();
		App->player->position.x = 0;
		App->player->position.y = 20;
		App->render->camera.x = 0;
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->fade->FadeToBlack(this, App->scene2);
		App->player->position.x = 0;
		App->player->position.y = 40;
		App->render->camera.x = 0;
	}

	// Control of the camera
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 5;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 5;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 5;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 5;

	// Camera control
	if (App->render->camera.x > CAMERA_LIMIT)  //need to put the X value in XML
	{
		App->render->camera.x = -App->player->position.x * 4 + 400;
		if (App->render->camera.x > 0)
			App->render->camera.x = 0;
	}

	// Limit player X position
	if (App->player->position.x > PLAYER_LIMIT)
		App->player->position.x = PLAYER_LIMIT;

	if (App->player->position.x < 0)
		App->player->position.x = 0;

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);

	return true;
}

// Called each loop iteration
bool j1Scene2::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene2::CleanUp()
{
	LOG("Freeing scene");

	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->player->CleanUp();

	return true;
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

void j1Scene2::ChangeScene()
{
	App->scene1->active = true;
	App->scene2->active = false;
	CleanUp();
	App->fade->FadeToBlack(App->scene2, App->scene1, 0.8f);
	App->player->Start();
	App->render->camera = { 0,0 };
	App->scene1->Start();
}
	

