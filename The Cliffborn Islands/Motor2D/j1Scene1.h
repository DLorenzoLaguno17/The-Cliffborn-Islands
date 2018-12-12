#ifndef __j1SCENE1_H__
#define __j1SCENE1_H__

#include "j1Module.h"
#include "j1Button.h"
#include "j1Timer.h"

struct SDL_Texture;
struct j1Settings;

class j1Scene1 : public j1Module
{
public:

	j1Scene1();

	// Destructor
	virtual ~j1Scene1();

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
	void ChangeSceneMenu();

	// Load and Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	fPoint initialScene1Position;
	bool player_created = false;

	void PlaceEnemies();

public:
	j1Settings * settings_window = nullptr;

private:

	SDL_Texture* debug_tex = nullptr;
	bool changingScene = false;
	bool resettingLevel = false;
	j1Timer	startup_time;
	int time_scene1;
	p2SString time_text;
	p2SString min_text;
	p2SString min_text_left = "0";
	j1Label* seconds;
	j1Label* minutes;
	_TTF_Font* text = nullptr;
	int min = 00;

};

#endif // __j1SCENE1_H__