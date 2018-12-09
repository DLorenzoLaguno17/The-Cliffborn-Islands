#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2List.h"

#define CURSOR_WIDTH 2

struct SDL_Texture;
struct _TTF_Font;
struct j1UserInterfaceElement;
struct SDL_Rect;
struct j1Button;

enum UIELEMENT_TYPES
{
	BUTTON,
	CHECKBOX,
	INPUTTEXT,
	LABEL,
	IMAGE
};

enum ButtonFunction {
	NO_FUNCTION = 0,
	PLAY_GAME,
	CLOSE_GAME,
	CREDITS,
	SAVE_GAME,
	LOAD_GAME,
};

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Factory method
	//j1UserInterfaceElement* CreateElement(UIELEMENT_TYPES type, int x, int y, SDL_Texture* text = nullptr);
	j1Button* CreateButton(UIELEMENT_TYPES type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text = nullptr, ButtonFunction function = NO_FUNCTION);

	void UpdateButtonsState();
	
	const SDL_Texture* GetAtlas() const;
	p2List<j1UserInterfaceElement*>	elements;
	p2List<j1Button*> buttons;

private:

	SDL_Texture * atlas;
	p2SString atlas_file_name;

};

#endif // __j1GUI_H__