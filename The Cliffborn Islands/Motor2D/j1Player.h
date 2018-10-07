#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Module.h"

struct SDL_Texture;

enum lastDirection {
	RIGHT,
	LEFT
};

class j1Player : public j1Module
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;

	// Animations of the player
	Animation idle_right;
	Animation idle_left;
	Animation run_right;
	Animation run_left;
	Animation jump_right;
	Animation jump_left;
	Animation fall_right;
	Animation fall_left;
	Animation attack_right;
	Animation attack_left;

	// To know the last direction the character was moving to
	lastDirection lastDirection = lastDirection::RIGHT;

	iPoint position;	

	int horizontalSpeed;
	int verticalSpeed;
	int initialVerticalSpeed;
	float gravity;

	uint currentTime;
	uint lastTime;
	
	bool updatedTime;
	bool dead = false;
};

#endif // __jPLAYER_H__
