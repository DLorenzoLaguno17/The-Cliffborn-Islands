#include "j1Settings.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Render.h"

j1Settings::j1Settings(UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text) :
	j1UserInterfaceElement(UIELEMENT_TYPES::BUTTON, x, y, text), section(section) {}

j1Settings::~j1Settings() {}

void j1Settings::Draw(float scale, int x, int y, bool use_camera)
{
	if (!use_camera)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_NONE, 1.0f, scale, INT_MAX, INT_MAX, false);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_NONE, 1.0f, scale);

}

bool j1Settings::CleanUp()
{
	LOG("Unloading button textures");
	App->tex->UnLoad(sprites);

	return true;
}