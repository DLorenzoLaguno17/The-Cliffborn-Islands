#ifndef __USERINTERFACEELEMENT_H__
#define __USERINTERFACEELEMENT_H__

#include "j1Gui.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;
class j1UserInterfaceElement {

public:
	//Constructor
	j1UserInterfaceElement(UIELEMENT_TYPES type, int x, int y, SDL_Texture* sprites) : type(type), position(x, y), sprites(sprites) {}

	~j1UserInterfaceElement() {}

	// Blits the element on the screen
	virtual void Draw(float scale, int x = 0, int y = 0, bool use_camera = true) {}

public:

	UIELEMENT_TYPES type;
	iPoint position;
	SDL_Texture* sprites = nullptr;
};

#endif // __USERINTERFACEELEMENT_H__