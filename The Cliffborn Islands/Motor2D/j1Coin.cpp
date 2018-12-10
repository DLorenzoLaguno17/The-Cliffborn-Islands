#include "j1Coin.h"
#include "j1Entity.h"

j1Coin::j1Coin(int x, int y, ENTITY_TYPES type)  : j1Entity(x, y, ENTITY_TYPES::HARPY)
{
	animation = NULL;

	idle.LoadEnemyAnimations("idle", "coin");

	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
	collider = nullptr;
}

j1Coin::~j1Coin() {}

bool j1Coin::Start()
{
	return false;
}

bool j1Coin::Update(float dt)
{
	return false;
}

bool j1Coin::CleanUp()
{
	return false;
}

bool j1Coin::Load(pugi::xml_node &)
{
	return false;
}

bool j1Coin::Save(pugi::xml_node &) const
{
	return false;
}
