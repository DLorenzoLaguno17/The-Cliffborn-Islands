#ifndef __USERINTERFACEELEMENT_H__
#define __USERINTERFACEELEMENT_H__

#include "j1Gui.h"
#include "p2Point.h" 

struct SDL_Texture;
struct SDL_Rect;
enum UIELEMENT_TYPES;

class j1UserInterfaceElement {

public:
	//Constructor
	j1UserInterfaceElement(int x, int y, UIELEMENT_TYPES type);

	//Destructor
	~j1UserInterfaceElement();

	// Blits the element on the screen
	virtual void Draw(SDL_Rect r, bool flip = false, int x = 0, int y = 0);

	// Executes aa action when the element is clicked
	virtual void OnClick();

private:

	UIELEMENT_TYPES type;
	iPoint position;
	SDL_Texture* sprites = nullptr;
};

#endif // __USERINTERFACEELEMENT_H__