#ifndef __j1HOOK_H__
#define __j1HOOK_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Module.h"

struct SDL_Texture;
struct Collider;

class j1Hook : public j1Module
{

public:

	j1Hook();

	// Destructor
	virtual ~j1Hook();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Called to check collisions
	void OnCollision(Collider* col_1, Collider* col_2);

public:

	SDL_Texture * graphics = nullptr;
	Animation* current_animation = nullptr;
	Collider* hookCollider = nullptr;

	Animation throwHook;
	Animation returnHook;
	Animation dragHookRight;
	Animation dragHookLeft;

	int objectivePosition = NULL;
	int hookSpeed;
	
	iPoint spawnPositionRight;
	iPoint spawnPositionLeft;
	iPoint hookPosition;
	iPoint hookSize;
	iPoint colliderPosition;

	bool somethingHit = false;
	bool thrown = false;
	bool arrived = false;
};

#endif // __j1HOOK_H__
