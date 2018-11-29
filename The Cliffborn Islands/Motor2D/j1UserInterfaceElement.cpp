#include "j1UserInterfaceElement.h"
#include "j1App.h"
#include "j1Render.h"

j1UserInterfaceElement::j1UserInterfaceElement(UIELEMENT_TYPES type, int x, int y, SDL_Texture* text, SDL_Rect* section) : 
	position(x, y), type(type), sprites(text), width(section->w), height(section->h) {}

void j1UserInterfaceElement::Draw(SDL_Rect r, bool flip, int x, int y)
{
	if (flip == false)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_NONE);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_HORIZONTAL);
}