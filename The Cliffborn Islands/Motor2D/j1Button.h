#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "j1UserInterfaceElement.h"
#include "p2Animation.h"

enum UIELEMENT_TYPES;
struct SDL_Texture;
enum ButtonFunction;

enum STATE {
	IDLE = 0,
	HOVERED,
	CLICKED,
	RELEASED
};

class j1Button : public j1UserInterfaceElement {
public:

	// Constructor
	// To create a button we need several parameters: its type, a texture, the three sections of the atlas the button will use and the function of the button
	j1Button(UIELEMENT_TYPES type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text = nullptr, ButtonFunction function = ButtonFunction::NO_FUNCTION);

	// Destructor
	virtual ~j1Button();

	// Called every frame
	bool Update(float dt, bool do_logic);

	// Called before quitting
	bool CleanUp();

public:
	STATE state = IDLE;
	ButtonFunction bfunction;

	Animation* animation = nullptr;
	SDL_Rect idle;
	SDL_Rect hovered;
	SDL_Rect clicked;
};

#endif // __BUTTON_H__