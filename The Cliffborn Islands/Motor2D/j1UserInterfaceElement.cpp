#include "j1UserInterfaceElement.h"
#include "j1App.h"
#include "j1Render.h"

j1UserInterfaceElement::j1UserInterfaceElement(UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text) : 
	type(type), position(x, y), section(section), sprites(text) {}

void j1UserInterfaceElement::Draw(float scale, bool flip, int x, int y)
{
	if (flip == false)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_NONE, 1.0f, scale);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_HORIZONTAL, 1.0f, scale);
}