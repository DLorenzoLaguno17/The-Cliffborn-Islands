#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1Harpy.h"
#include "j1Player.h"
#include "j1Hook.h"

j1EntityManager::j1EntityManager()
{
	name.create("entity");
}

j1EntityManager::~j1EntityManager() {}

bool j1EntityManager::Start()
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{ 
		iterator->data->Start();
	}

	return true;
}

bool j1EntityManager::PreUpdate()
{	
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::UNKNOWN)
		{
			SpawnEnemy(queue[i]);
			queue[i].type = ENTITY_TYPES::UNKNOWN;		
		}
	}

	return true;
}

bool j1EntityManager::Update(float dt)
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->Update(dt);
	}

	return true;
}

bool j1EntityManager::PostUpdate()
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	LOG("Freeing all enemies");

	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->CleanUp();
		int index = entities.find(iterator->data);
		RELEASE(entities.At(index)->data);
		entities.del(entities.At(index));
	}

	player = nullptr;
	hook = nullptr;

	return true;
}

j1Entity* j1EntityManager::CreateEntity(ENTITY_TYPES type, int x, int y)
{
	j1Entity* ret = nullptr;
	switch (type)
	{
	case PLAYER: 
		ret = new j1Player(x, y, type);
	if (ret != nullptr) entities.add(ret); break;

	case HOOK:
		ret = new j1Hook(x, y, type);
		if (ret != nullptr) entities.add(ret); break; 
	}
	return ret;
}

void j1EntityManager::AddEnemy(int x, int y, ENTITY_TYPES type)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type == ENTITY_TYPES::UNKNOWN)
		{
			queue[i].type = type;
			queue[i].position.x = x;
			queue[i].position.y = y;
			break;
		}
	}
}

void j1EntityManager::SpawnEnemy(const EnemyInfo& info)
{

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::UNKNOWN)
		{
			j1Entity* entity;
			if (queue[i].type == HARPY) 
				entity = new j1Harpy(info.position.x, info.position.y, info.type); 

			entities.add(entity);
			entity->Start();
		}
	}
}

void j1EntityManager::CreatePlayer() 
{
	player = (j1Player*)CreateEntity(PLAYER);
	hook = (j1Hook*)CreateEntity(HOOK);
}

void j1EntityManager::OnCollision(Collider* c1, Collider* c2)
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != NULL; iterator = iterator->next)
	{
		if (iterator->data->collider == c1) 
		{ 
			iterator->data->OnCollision(c1, c2); 	
			break; 
		}
	}
}

bool j1EntityManager::Load(pugi::xml_node& data)
{
	if (player != nullptr)
	{
		player->Load(data);
	}

	for (pugi::xml_node harpy = data.child("harpy").child("position"); harpy; harpy = harpy.next_sibling()) {
		iPoint harpypos = { harpy.attribute("x").as_int(), harpy.attribute("y").as_int() };
		AddEnemy(harpypos.x, harpypos.y, HARPY);
	}

	return true;
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	if (player != nullptr)
	{
		player->Save(data);
	}

	pugi::xml_node harpy = data.append_child("harpy");

	for (p2List_item<j1Entity*>* iterator = entities.start; iterator; iterator = iterator->next)
	{
		if (iterator->data->type == HARPY)
			iterator->data->Save(harpy);
	}

	return true;
}