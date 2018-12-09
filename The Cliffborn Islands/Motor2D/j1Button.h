#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "j1UserInterfaceElement.h"
#include "p2Animation.h"

enum UIELEMENT_TYPES;
struct SDL_Texture;

enum STATE {
	IDLE = 0,
	HOVERED,
	CLICKED,
	RELEASED
};

class j1Button : public j1UserInterfaceElement {
public:

	// Constructor
	j1Button(UIELEMENT_TYPES type, int x, int y, SDL_Texture* text = nullptr);

	// Destructor
	virtual ~j1Button();

	// Called every frame
	bool Update(float dt, bool do_logic);

	// Called before quitting
	bool CleanUp();

public:
	STATE state = IDLE;

	Animation* animation = nullptr;
	Animation idle;
	Animation hovered;
	Animation clicked;
};

#endif // __BUTTON_H__