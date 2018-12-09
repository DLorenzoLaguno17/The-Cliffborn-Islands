#include "j1Button.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Log.h"

#include "Brofiler/Brofiler.h"

j1Button::j1Button(UIELEMENT_TYPES type, int x, int y, SDL_Texture* text) :
	j1UserInterfaceElement(UIELEMENT_TYPES::BUTTON, x, y, text) {}

j1Button::~j1Button() {}

bool j1Button::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("Button", Profiler::Color::LightSeaGreen)	

	// Drawing the button
	SDL_Rect r = animation->GetCurrentFrame(dt);
	Draw(r);

	return true;
}

bool j1Button::CleanUp()
{
	LOG("Unloading harpy");
	App->tex->UnLoad(sprites);

	return true;
}