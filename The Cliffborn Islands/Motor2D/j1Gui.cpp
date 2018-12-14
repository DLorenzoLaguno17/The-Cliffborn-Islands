#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Button.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Audio.h"
#include "j1SceneMenu.h"
#include "j1SceneCredits.h"
#include "j1Scene1.h"
#include "j1Scene2.h"

#include "Brofiler/Brofiler.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& config)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = config.child("atlas").attribute("file").as_string("");
	buttonsScale = config.child("scale").attribute("buttonsScale").as_float();
	boxScale = config.child("scale").attribute("boxScale").as_float();

	// Copying box spawn position
	settingsPosition = { 54, 10 };

	return ret;
}	

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	// Audios are loaded
	LOG("Loading player audios");
	if (!loadedAudios) {
		clickSound = App->audio->LoadFx("audio/fx/click.wav");
		hoverSound = App->audio->LoadFx("audio/fx/hover.wav");
		loadedAudios = true;
	}

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	BROFILER_CATEGORY("GuiPreUpdate", Profiler::Color::Orange)
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	BROFILER_CATEGORY("GuiPostUpdate", Profiler::Color::Yellow)

	// Blitting settings window
	if (App->scene1->settings_window != nullptr && App->scene1->settings_window->visible == true)
		App->scene1->settings_window->Draw(App->gui->boxScale);

	if (App->scene2->settings_window != nullptr && App->scene2->settings_window->visible == true)
		App->scene2->settings_window->Draw(App->gui->boxScale);

	return true;
}

// Factiry methods
j1Button* j1Gui::CreateButton(p2List<j1Button*>* buttons, UIELEMENT_TYPES type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text, ButtonFunction function) {
	j1Button* ret = nullptr;

	ret = new j1Button(type, x, y, idle, hovered, clicked, text, function);
	if (ret != nullptr) buttons->add(ret);

	return ret;
}

j1Label* j1Gui::CreateLabel(p2List<j1Label*>* labels, UIELEMENT_TYPES type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color) {
	j1Label* ret = nullptr;

	ret = new j1Label(type, x, y, font, text, color);
	if (ret != nullptr) labels->add(ret);

	return ret;
}

j1Box* j1Gui::CreateBox(UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text) {
	j1Box* ret = nullptr;

	ret = new j1Box(type, x, y, section, text);

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

void j1Gui::UpdateButtonsState(p2List<j1Button*>* buttons) {
	int x, y; App->input->GetMousePosition(x, y);

	for (p2List_item<j1Button*>* button = buttons->start; button != nullptr; button = button->next) {
		if (x <= button->data->position.x + button->data->situation.w * App->gui->buttonsScale && x >= button->data->position.x
			&& y <= button->data->position.y + button->data->situation.h * App->gui->buttonsScale && y >= button->data->position.y) {

			if(App->credits->active == false && App->menu->settings_window->visible
				&& button->data->bfunction != CLOSE_GAME && button->data->bfunction != SETTINGS) continue;

			button->data->state = STATE::HOVERED;
			if (!button->data->hoverPlayed) {
				App->audio->PlayFx(hoverSound);
				button->data->hoverPlayed = true;
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				button->data->state = STATE::CLICKED;
				if (!button->data->clickPlayed) {
					App->audio->PlayFx(clickSound);
					button->data->clickPlayed = true;
				}
			}
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				button->data->state = STATE::RELEASED;
				button->data->clickPlayed = false;
			}
		}
		else {
			button->data->state = STATE::IDLE;
			button->data->hoverPlayed = false;
		}
	}
}

void j1Gui::UpdateBoxesState() {
	int x, y; App->input->GetMousePosition(x, y);

	j1Box* aux = nullptr;

	// Checking which window is enabled
	if (App->menu->active && App->menu->settings_window->visible)
		aux = App->menu->settings_window;
	else if (App->scene1->active && App->scene1->settings_window->visible)
		aux = App->scene1->settings_window;
	else if(App->scene2->active && App->scene2->settings_window->visible)
		aux = App->scene2->settings_window;

	// Checking if it is being dragged
	if (aux != nullptr && x <= aux->position.x + aux->section.w * App->gui->boxScale && x >= aux->position.x
		&& y <= aux->position.y + aux->section.h * App->gui->boxScale && y >= aux->position.y){
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			aux->clicked = true;
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			aux->clicked = false;
	}
}

// class Gui ---------------------------------------------------