#include "j1UserInterfaceElement.h"
#include "j1App.h"
#include "j1Render.h"

j1UserInterfaceElement::j1UserInterfaceElement(int x, int y, UIELEMENT_TYPES type) : position(x, y), type(type){}

void j1UserInterfaceElement::OnClick() {}

void j1UserInterfaceElement::Draw(SDL_Rect r, bool flip, int x, int y)
{
	if (flip == false)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_NONE);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_HORIZONTAL);
}