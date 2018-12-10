#ifndef __LABEL_H__
#define __LABEL_H__

#include "j1UserInterfaceElement.h"

struct _TTF_Font;

class j1Label : public j1UserInterfaceElement {
public:
	// Constructor
	j1Label(UIELEMENT_TYPES type, int x, int y, _TTF_Font* font = nullptr);

	// Destructor
	virtual ~j1Label();

	// Blits the element on the screen
	void Draw(float scale, const char* text, SDL_Color color = { 255, 255, 255, 255 }, bool flip = false, int x = 0, int y = 0);

public:
	_TTF_Font* font;
};

#endif // __LABEL_H__