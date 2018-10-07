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
	//bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

	/*bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);*/

public:
	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;

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

	bool dead = false;
};

#endif // __jPLAYER_H__
