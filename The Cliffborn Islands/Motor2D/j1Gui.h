#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2List.h"
#include "SDL/include/SDL_rect.h"

#define CURSOR_WIDTH 2

struct SDL_Texture;
struct _TTF_Font;
struct SDL_Rect;
struct j1Button;
struct j1Label;

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
	CONTINUE,
	SETTINGS
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

	// Factory methods
	j1Button* CreateButton(UIELEMENT_TYPES type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text = nullptr, ButtonFunction function = NO_FUNCTION);
	j1Label* CreateLabel(UIELEMENT_TYPES type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color = { 255, 255, 255, 255});

	void UpdateButtonsState();
	
	const SDL_Texture* GetAtlas() const;

public:
	p2List<j1Button*> buttons;
	p2List<j1Label*> labels;

private:

	SDL_Texture* atlas = nullptr;
	p2SString atlas_file_name;

};

#endif // __j1GUI_H__