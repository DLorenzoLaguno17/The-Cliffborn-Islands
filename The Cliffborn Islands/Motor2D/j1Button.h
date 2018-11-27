#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "j1UserInterfaceElement.h"

class j1Button : public j1UserInterfaceElement {
public:
	enum STATE {
		IDLE,
		HOVERED,
		CLICKED,
		RELEASED
	};

	// Constructor
	j1Button(int x, int y, UIELEMENT_TYPES type);

	// Destructor
	virtual ~j1Button();

	void Draw();
	void OnClick();
};

#endif // __BUTTON_H__