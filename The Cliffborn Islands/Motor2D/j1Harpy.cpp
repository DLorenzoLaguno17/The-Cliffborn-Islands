#include "j1Harpy.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene1.h"

j1Harpy::j1Harpy() : j1Module()
{
	current_animation = NULL;

	idle.LoadEnemyAnimations("idle", "harpy");
	move.LoadEnemyAnimations("move", "harpy");

	name.create("harpy");
}

j1Harpy::~j1Harpy() {}

bool j1Harpy::Awake(pugi::xml_node& config)
{
	speed = config.attribute("speed").as_int();

	initialPosition.x = config.child("position").attribute("x").as_int();
	initialPosition.y = config.child("position").attribute("y").as_int();

	return true;
}

bool j1Harpy::Start()
{
	// Textures are loaded
	LOG("Loading harpy textures");
	graphics = App->tex->Load("textures/enemies/harpy/harpy.png");

	current_animation = &idle;

	// Setting harpy position
	//position.x = initialPosition.x;
	//position.y = initialPosition.y;
	position.x = 250;
	position.y = 20;

	harpy = App->collisions->AddCollider({ (int)position.x - 10, (int)position.y - 10, 28, 19 }, COLLIDER_HARPY, this);

	return true;
}

bool j1Harpy::Update(float dt)
{
	harpy->SetPos(position.x, position.y);

	if ((App->player->position.x - position.x) <= DETECTION_RANGE && (App->player->position.x - position.x) >= -DETECTION_RANGE && App->player->player->type == COLLIDER_PLAYER)
	{
		iPoint origin = { App->map->WorldToMap((int)position.x + 16, (int)position.y - 16)};
		iPoint destination = { App->map->WorldToMap((int)App->player->position.x + 8,(int)App->player->position.y) };

		path = App->path->CreatePath(origin, destination);
		Move(*path, dt);
		path_created = true;
	}

	else if (path_created)
		path->Clear();
	
	SDL_Rect character = current_animation->GetCurrentFrame();

	if (position.x - App->player->position.x >= 0)
		App->render->Blit(graphics, (int)position.x - 10, (int)position.y - 10, &character, SDL_FLIP_HORIZONTAL);
	else
		App->render->Blit(graphics, (int)position.x - 10, (int)position.y - 10, &character, SDL_FLIP_NONE);

	return true;
}

bool j1Harpy::CleanUp()
{
	LOG("Unloading harpy");
	App->tex->UnLoad(graphics);

	return true;
}

void j1Harpy::OnCollision(Collider * col_1, Collider * col_2)
{
	if (col_2->type == COLLIDER_PLAYER || col_1->type == COLLIDER_PLAYER)
	{
		App->fade->FadeToBlack(App->scene1, App->scene1);
		App->player->position = App->player->initialPosition;
		path->Clear();
		position = { 250, 20 };
	}
}

bool j1Harpy::Load(pugi::xml_node &)
{
	return true;
}

bool j1Harpy::Save(pugi::xml_node &) const
{
	return true;
}

void j1Harpy::Move(p2DynArray<iPoint>& path, float dt)
{
	speed = 1.0f;
	direction = App->path->CheckDirection(path);

	if (direction == Movement::DOWN)
	{
		current_animation = &move;
		position.y += speed;
	}
	
	if (direction == Movement::UP)
	{
		current_animation = &move;
		position.y -= speed;
	}

	if (direction == Movement::RIGHT)
	{
		current_animation = &move;
		position.x += speed;
	}
	
	if (direction == Movement::LEFT)
	{
		current_animation = &move;
		position.x -= speed;
	}
}

