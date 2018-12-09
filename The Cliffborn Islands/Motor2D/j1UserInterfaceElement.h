#ifndef __USERINTERFACEELEMENT_H__
#define __USERINTERFACEELEMENT_H__

#include "j1Gui.h"
#include "p2Point.h"

struct SDL_Texture;
struct SDL_Rect;

class j1UserInterfaceElement {

public:
	//Constructor
	j1UserInterfaceElement(UIELEMENT_TYPES type, int x, int y, SDL_Texture* text = nullptr);

	~j1UserInterfaceElement() {}

	// Blits the element on the screen
	virtual void Draw(SDL_Rect r, bool flip = false, int x = 0, int y = 0);

public:

	UIELEMENT_TYPES type;
	iPoint position;
	uint width;
	uint height;
	SDL_Texture* sprites = nullptr;
};

#endif // __USERINTERFACEELEMENT_H__