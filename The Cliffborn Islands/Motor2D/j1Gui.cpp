#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Button.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	return true;
}

/*j1UserInterfaceElement* j1Gui::CreateElement(UIELEMENT_TYPES type, int x, int y, SDL_Texture* text){
	j1UserInterfaceElement* ret = nullptr;
	switch (type)
	{
	case BUTTON:
		ret = new j1Button(type, x, y, text);
		if (ret != nullptr) buttons.add(ret); break;
	}
	return ret;
}*/

j1Button* j1Gui::CreateButton(UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text, ButtonFunction function) {
	j1Button* ret = nullptr;

	ret = new j1Button(type, x, y, section, text, function);
	if (ret != nullptr) buttons.add(ret);

	return ret;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::UpdateButtonsState() {
	int x, y; App->input->GetMousePosition(x, y);

	for (p2List_item<j1Button*>* button = buttons.start; button != nullptr; button = button->next) {
		if (x <= button->data->position.x + button->data->section.w && x >= button->data->position.x
			&& y <= button->data->position.y + button->data->section.h && y >= button->data->position.y) {
			button->data->state = STATE::HOVERED;
			button->data->animation = &button->data->hovered;

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				button->data->state = STATE::CLICKED;
				button->data->animation = &button->data->clicked;
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP){
				button->data->state = STATE::RELEASED;
				button->data->animation = &button->data->idle;
			}
		}
		else {
			button->data->state = STATE::IDLE;
			button->data->animation = &button->data->idle;
		}
	}
}

// class Gui ---------------------------------------------------