#ifndef __j1HARPY_H__
#define __j1HARPY_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Module.h"
#include "p2DynArray.h"
#include "j1Pathfinding.h"

#define DETECTION_RANGE 80

struct SDL_Texture;
//struct Collider;

class j1Harpy : public j1Module
{
public:
	j1Harpy();

	// Destructor
	virtual ~j1Harpy();

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

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void Move(p2DynArray<iPoint>& path, float dt);

public:
	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;

	Animation idle;
	Animation move;

	Collider* harpy = nullptr;

public:
	fPoint initialPosition;
	fPoint position;

	iPoint margin;
	iPoint colliderSize;
	
	float speed;

	Movement direction;

	p2DynArray<iPoint>* path;

	bool path_created = false;

};
#endif // __j1HARPY_H__