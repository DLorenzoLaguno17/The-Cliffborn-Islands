#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "j1UserInterfaceElement.h"
#include "p2Point.h"

enum UIELEMENT_TYPES;
struct SDL_Texture;

class j1Settings : public j1UserInterfaceElement {
public:

	// Constructor
	// To create a button we need several parameters: its type, a texture, the three sections of the atlas the button will use and the function of the button
	j1Settings(UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text = nullptr);

	// Destructor
	virtual ~j1Settings();

	// Called before quitting
	bool CleanUp();

	// Blits the element on the screen
	void Draw(float scale = 1.0f, int x = 0, int y = 0, bool use_camera = true) override;

public:
	SDL_Rect section;
	bool visible = false;
	bool clicked = false;

	bool distanceCalculated = false;
	iPoint mouseDistance;
};

#endif // __SETTINGS_H__