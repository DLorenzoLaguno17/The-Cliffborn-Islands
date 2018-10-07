#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Render.h"

j1Player::j1Player() : j1Module()
{
	current_animation = NULL;

	// Idle animations
	idle_right.PushBack({ 2, 184, 22, 25 });
	idle_right.PushBack({ 33, 184, 24, 25 });
	idle_right.PushBack({ 65, 184, 24, 25 });
	idle_right.PushBack({ 97, 184, 24, 25 });
	idle_right.PushBack({ 130, 184, 22, 25 });
	idle_right.PushBack({ 163, 184, 21, 25 });
	idle_right.loop = true;
	idle_right.speed = 0.8f;

	idle_left.PushBack({ 163, 214, 22, 25 });
	idle_left.PushBack({ 130, 214, 24, 25 });
	idle_left.PushBack({ 98, 214, 24, 25 });
	idle_left.PushBack({ 66, 214, 24, 25 });
	idle_left.PushBack({ 35, 214, 22, 25 });
	idle_left.PushBack({ 3, 214, 21, 25 });
	idle_left.loop = true;
	idle_left.speed = 0.8f;

	// Runnig animations
	run_right.PushBack({ 1, 96, 23, 25 });
	run_right.PushBack({ 32, 99, 25, 25 });
	run_right.PushBack({ 65, 99, 23, 25 });
	run_right.PushBack({ 100, 98, 20, 25 });
	run_right.PushBack({ 133, 97, 20, 25 });
	run_right.PushBack({ 165, 99, 17, 25 });
	run_right.PushBack({ 199, 99, 18, 25 });
	run_right.PushBack({ 228, 98, 20, 25 });
	run_right.loop = true;
	run_right.speed = 0.8f;	

	run_left.PushBack({ 227, 127, 23, 25 });
	run_left.PushBack({ 194, 130, 25, 25 });
	run_left.PushBack({ 163, 130, 23, 25 });
	run_left.PushBack({ 131, 129, 20, 25 });
	run_left.PushBack({ 98, 129, 20, 25 });
	run_left.PushBack({ 69, 130, 17, 25 });
	run_left.PushBack({ 34, 130, 18, 25 });
	run_left.PushBack({ 3, 129, 20, 25 });
	run_left.loop = true;
	run_left.speed = 0.8f;

	// Jump animations
	jump_right.PushBack({ 2, 158, 20, 25 });
	jump_right.PushBack({ 34, 158, 20, 25 });
	jump_right.loop = true;
	jump_right.speed = 0.8f;

	fall_right.PushBack({ 87, 157, 22, 26 });
	fall_right.PushBack({ 119, 157, 22, 26 });
	fall_right.loop = true;
	fall_right.speed = 0.8f;

	jump_left.PushBack({ 272, 158, 20, 25 });
	jump_left.PushBack({ 240, 158, 20, 25 });
	jump_left.loop = true;
	jump_left.speed = 0.8f;

	fall_left.PushBack({ 185, 157, 22, 26 });
	fall_left.PushBack({ 153, 157, 22, 26 });
	fall_left.loop = true;
	fall_left.speed = 0.8f;
	
	// Attack animations
	attack_right.PushBack({ 1, 272, 29, 27 });
	attack_right.PushBack({ 64, 272, 29, 27 });
	attack_right.PushBack({ 135, 272, 35, 27 });
	attack_right.PushBack({ 198, 272, 40, 27 });
	attack_right.PushBack({ 253, 272, 42, 27 });
	attack_right.loop = false;
	attack_right.speed = 0.5f;
	
	attack_right.PushBack({ 269, 244, 29, 27 });
	attack_right.PushBack({ 206, 244, 29, 27 });
	attack_right.PushBack({ 129, 244, 35, 27 });
	attack_right.PushBack({ 61, 244, 40, 27 });
	attack_right.PushBack({ 5, 244, 42, 27 });
	attack_left.loop = false;
	attack_left.speed = 0.5f;

	name.create("player");
}

j1Player::~j1Player() {}

// Reading from config file
bool j1Player::Awake(pugi::xml_node& config) {
	// Setting the position of the player
	/*position.x = config.child("position").attribute("x").as_int();
	position.y = config.child("position").attribute("y").as_int();*/

	return true;
}

// Load assets
bool j1Player::Start() {
	
	// Textures are loaded
	LOG("Loading player textures");
	graphics = App->tex->Load("textures/character/character.png");
	
	return true;
}

//Call modules before each loop iteration
bool j1Player::PreUpdate() {

	return true;
}

// Call modules on each loop iteration
bool j1Player::Update() {

	// Control of the player
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		position.x -= 1;
		lastDirection = lastDirection::LEFT;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		position.x += 1;
		lastDirection = lastDirection::RIGHT;
	}

	current_animation = &idle_right;

	SDL_Rect rect = current_animation->GetCurrentFrame();

	// Blitting the player
	App->render->Blit(graphics, 0, 0, &rect);

	return true;
}

// Call modules after each loop iteration
bool j1Player::PostUpdate() {

	return true;
}

// Load game state
bool j1Player::Load(pugi::xml_node& data) {

	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();

	return true;
}

// Save game state
bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node player = data.append_child("position");

	player.append_attribute("x") = position.x;
	player.append_attribute("y") = position.y;

	return true;
}

// Called before quitting
bool j1Player::CleanUp() {
	
	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(graphics);

	return true;
}