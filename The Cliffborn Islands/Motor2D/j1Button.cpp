#include "j1Button.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Render.h"

j1Button::j1Button(UIELEMENT_TYPES type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text, ButtonFunction function) :
	j1UserInterfaceElement(UIELEMENT_TYPES::BUTTON, x, y), sprites(text), idle(idle), hovered(hovered), clicked(clicked), bfunction(function) {}

j1Button::~j1Button() {}

void j1Button::Draw(float scale, bool flip, int x, int y)
{
	if (flip == false)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &situation, SDL_FLIP_NONE, 1.0f, scale);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &situation, SDL_FLIP_HORIZONTAL, 1.0f, scale);
}

bool j1Button::CleanUp()
{
	LOG("Unloading button textures");
	App->tex->UnLoad(sprites);

	return true;
}