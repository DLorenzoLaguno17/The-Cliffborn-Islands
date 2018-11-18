#ifndef __j1SCENE2_H__
#define __j1SCENE2_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene2 : public j1Module
{
public:

	j1Scene2();

	// Destructor
	virtual ~j1Scene2();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Called to change scene
	void ChangeScene();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void PlaceEnemies();

	fPoint initialScene2Position;
private:

	bool player_created = false;

	SDL_Texture* debug_tex;
};

#endif // __j1SCENE2_H__