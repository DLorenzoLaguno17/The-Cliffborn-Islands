#include "j1Button.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Log.h"

#include "Brofiler/Brofiler.h"

j1Button::j1Button(UIELEMENT_TYPES type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text, ButtonFunction function) :
	j1UserInterfaceElement(UIELEMENT_TYPES::BUTTON, x, y, idle, text), idle(idle), hovered(hovered), clicked(clicked), bfunction(function) {}

j1Button::~j1Button() {}

bool j1Button::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("Button", Profiler::Color::LightSeaGreen)	

	// Drawing the button
	Draw();

	return true;
}

bool j1Button::CleanUp()
{
	LOG("Unloading harpy");
	App->tex->UnLoad(sprites);

	return true;
}