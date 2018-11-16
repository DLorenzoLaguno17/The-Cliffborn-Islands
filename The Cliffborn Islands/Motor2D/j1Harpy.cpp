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

#include "Brofiler/Brofiler.h"

j1Harpy::j1Harpy(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::HARPY)
{
	animation = NULL;

	idle.LoadEnemyAnimations("idle", "harpy");
	move.LoadEnemyAnimations("move", "harpy");

	// Setting harpy position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1Harpy::~j1Harpy() {}

bool j1Harpy::Start()
{
	// Textures are loaded
	LOG("Loading harpy textures");
	sprites = App->tex->Load("textures/enemies/harpy/harpy.png");

	LoadHarpyProperties();

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x - margin.x, (int)position.y - margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1Harpy::Update(float dt)
{
	BROFILER_CATEGORY("HarpyUpdate", Profiler::Color::LightSeaGreen)

	collider->SetPos(position.x, position.y);

	if ((App->entity->player->position.x - position.x) <= DETECTION_RANGE && (App->entity->player->position.x - position.x) >= -DETECTION_RANGE && App->entity->player->collider->type == COLLIDER_PLAYER)
	{
		iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y / 2) };
		iPoint destination;
		if (position.x < App->entity->player->position.x)
			destination = { App->map->WorldToMap((int)App->entity->player->position.x + App->entity->player->playerSize.x + 1, (int)App->entity->player->position.y + App->entity->player->playerSize.y / 2) };
		else
			destination = { App->map->WorldToMap((int)App->entity->player->position.x, (int)App->entity->player->position.y + App->entity->player->playerSize.y / 2) };

		if (!App->entity->player->dead && App->path->IsWalkable(destination) && App->path->IsWalkable(origin))
		{
			path = App->path->CreatePath(origin, destination);
			Move(*path, dt);
			path_created = true;
		}
	}

	else if (path_created)
		path->Clear();

	if (App->entity->player->position == App->entity->player->initialPosition)
		position = initialPosition;

	if (position.x - App->entity->player->position.x >= 0)
		Draw(-10, -10, true);
	else
		Draw(-10, -10, false);

	return true;
}

bool j1Harpy::CleanUp()
{
	LOG("Unloading harpy");
	App->tex->UnLoad(sprites);
	if (collider != nullptr)
		collider->to_delete = true;

	return true;
}

void j1Harpy::OnCollision(Collider * col_1, Collider * col_2)
{
}

bool j1Harpy::Load(pugi::xml_node &)
{
	return true;
}

bool j1Harpy::Save(pugi::xml_node &) const
{
	return true;
}

void j1Harpy::LoadHarpyProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node harpy;
	harpy = config.child("harpy");

	speed = harpy.attribute("speed").as_int();

	// Copying the values of the collider
	margin.x = harpy.child("margin").attribute("x").as_int();
	margin.y = harpy.child("margin").attribute("y").as_int();
	colliderSize.x = harpy.child("colliderSize").attribute("w").as_int();
	colliderSize.y = harpy.child("colliderSize").attribute("h").as_int();
}

void j1Harpy::Move(p2DynArray<iPoint>& path, float dt)
{
	direction = App->path->CheckDirection(path);

	if (direction == Movement::DOWN_RIGHT)
	{
		animation = &move;
		position.y += speed * dt;
		position.x += speed * dt;
	}

	else if (direction == Movement::DOWN_LEFT)
	{
		animation = &move;
		position.y += speed * dt;
		position.x -= speed * dt;
	}

	else if (direction == Movement::UP_RIGHT)
	{
		animation = &move;
		position.y -= speed * dt;
		position.x += speed * dt;
	}

	else if (direction == Movement::UP_LEFT)
	{
		animation = &move;
		position.y -= speed * dt;
		position.x -= speed * dt;
	}

	else if (direction == Movement::DOWN)
	{
		animation = &move;
		position.y += speed * dt;
	}

	else if (direction == Movement::UP)
	{
		animation = &move;
		position.y -= speed * dt;
	}

	else if (direction == Movement::RIGHT)
	{
		animation = &move;
		position.x += speed * dt;
	}

	else if (direction == Movement::LEFT)
	{
		animation = &move;
		position.x -= speed * dt;
	}
}