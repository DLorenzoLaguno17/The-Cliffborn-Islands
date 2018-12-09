#ifndef __j1SCENEMENU_H__
#define __j1SCENEMENU_H__

#include "j1Module.h"
#include "j1Render.h"
#include "p2List.h"
#include "j1Button.h"

struct SDL_Texture;

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
	void ChangeScene();

private:

	SDL_Texture* gui_tex = nullptr;
	bool continueGame = true;
	p2List<j1Button*> menuButtons;
	j1UserInterfaceElement* testButton = nullptr;
};

#endif //__j1SCENEMENU_H__