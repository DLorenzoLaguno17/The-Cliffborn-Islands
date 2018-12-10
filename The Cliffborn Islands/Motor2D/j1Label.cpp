#include "j1Label.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "p2Log.h"

#include "Brofiler/Brofiler.h"

j1Label::j1Label(UIELEMENT_TYPES type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color) :
	j1UserInterfaceElement(UIELEMENT_TYPES::LABEL, x, y), font(font), text(text), color(color) {}

j1Label::~j1Label() {}

void j1Label::Draw(float scale, bool flip, int x, int y)
{
	SDL_Texture* tex = App->font->Print(text, color, font);

	if (flip == false)
		App->render->Blit(tex, (int)position.x + x, (int)position.y + y);
}