#ifndef __j1SCENEMENU_H__
#define __j1SCENEMENU_H__

#include "j1Module.h"
#include "j1Render.h"
#include "p2List.h"
#include "p2Animation.h"

struct SDL_Texture;
struct _TTF_Font;
struct j1Settings;

class j1SceneMenu : public j1Module
{
public:
	j1SceneMenu();

	virtual ~j1SceneMenu();

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
	void ChangeScene(bool scene1);

	// Load and Save
	bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

public:
	j1Settings* settings_window = nullptr;

private:

	SDL_Texture* gui_tex = nullptr;
	SDL_Texture* logo_tex = nullptr;
	SDL_Texture* player_tex = nullptr;
	SDL_Texture* harpy_tex = nullptr;

	_TTF_Font* font = nullptr;

	bool continueGame = true;
	bool startGame = false;
	bool player_created = false;

	Animation player;
	Animation harpy;
};

#endif //__j1SCENEMENU_H__