#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Module.h"

struct SDL_Texture;
struct Collider;

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

	// Called to check collisions
	void OnCollision(Collider* col_1, Collider* col_2);

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;
	Collider* player = nullptr;
	Collider* futurePlayer = nullptr;

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
	Animation godmode_right;
	Animation godmode_left;

	// To know the last direction the character was moving to
	lastDirection lastDirection = lastDirection::RIGHT;
	
	fPoint initialPosition;
	fPoint position;
	iPoint playerSize;
	iPoint margin;

	uint currentJumps;
	uint initialJumps;

	float godModeSpeed;
	float horizontalSpeed;
	//Jumping speed
	float initialVerticalSpeed;
	float verticalSpeed;
	// Free fall speed
	float fallingSpeed;
	float initialFallingSpeed;
	// "Gravity"
	float verticalAcceleration;

	SDL_Rect intersectingRect;

	// It tells you wether the player has landed, has a wall in front, a wall behind or a wall above
	bool feetOnGround = false;
	bool wallInFront = false;
	bool wallBehind = false;
	bool wallAbove = false;

	bool GodMode = false;
	bool playerIdle = false;
	bool jumping = false;
	bool freefall = false;
	bool dead = false;

};

#endif // __jPLAYER_H__
