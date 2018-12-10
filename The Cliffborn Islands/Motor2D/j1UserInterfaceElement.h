#ifndef __USERINTERFACEELEMENT_H__
#define __USERINTERFACEELEMENT_H__

#include "j1Gui.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;
class j1UserInterfaceElement {

public:
	//Constructor
	j1UserInterfaceElement(UIELEMENT_TYPES type, int x, int y) : type(type), position(x, y) {}

	~j1UserInterfaceElement() {}

	// Blits the element on the screen
	virtual void Draw(float scale, bool flip = false, int x = 0, int y = 0) {}

public:

	UIELEMENT_TYPES type;
	iPoint position;
};

#endif // __USERINTERFACEELEMENT_H__