#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2List.h"

#define CURSOR_WIDTH 2

struct SDL_Texture;
struct _TTF_Font;
struct j1UserInterfaceElement;
struct SDL_Rect;

enum UIELEMENT_TYPES
{
	BUTTON,
	CHECKBOX,
	INPUTTEXT,
	LABEL,
	IMAGE
};

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Factory method
	j1UserInterfaceElement* CreateElement(UIELEMENT_TYPES type, int x, int y, SDL_Texture* text = nullptr);
	
	const SDL_Texture* GetAtlas() const;
	p2List<j1UserInterfaceElement*>	elements;

private:

	SDL_Texture * atlas;
	p2SString atlas_file_name;

};

#endif // __j1GUI_H__