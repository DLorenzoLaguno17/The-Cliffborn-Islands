#ifndef __LABEL_H__
#define __LABEL_H__

#include "j1UserInterfaceElement.h"

class j1Label : public j1UserInterfaceElement {
public:
	// Constructor
	j1Label(int x, int y, UIELEMENT_TYPES type);

	// Destructor
	~j1Label();

	void Draw();
};

#endif // __LABEL_H__