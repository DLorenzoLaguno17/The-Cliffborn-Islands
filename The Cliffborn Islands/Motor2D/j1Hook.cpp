#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Hook.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Scene1.h"
#include "j1Scene2.h"

j1Hook::j1Hook() : j1Module()
{
	current_animation = NULL;

	throwHook.LoadAnimations("throwHook");
	returnHook.LoadAnimations("returnHook");
	dragHookRight.LoadAnimations("dragHookRight");
	dragHookLeft.LoadAnimations("dragHookLeft");

	name.create("hook");
}

j1Hook::~j1Hook() {}

// Reading from config file
bool j1Hook::Awake(pugi::xml_node& config) {

	// Copying variables
	hookSpeed = config.attribute("speed").as_int();

	// Copying the size of the collider
	hookSize.x = config.child("size").attribute("width").as_int();
	hookSize.y = config.child("size").attribute("height").as_int();

	// Copying the spawning positions
	spawnPositionRight.x = config.child("spawnPositionRight").attribute("x").as_int();
	spawnPositionRight.y = config.child("spawnPositionRight").attribute("y").as_int();
	spawnPositionLeft.x = config.child("spawnPositionLeft").attribute("x").as_int();
	spawnPositionLeft.y = config.child("spawnPositionLeft").attribute("y").as_int();

	return true;
}

// Load assets
bool j1Hook::Start() {

	current_animation = NULL;

	// Textures are loaded
	LOG("Loading player textures");
	graphics = App->tex->Load("textures/character/hook.png");

	current_animation = &throwHook;
	
	hookCollider = App->collisions->AddCollider({0, 0, hookSize.x, hookSize.y}, COLLIDER_HOOK, this);

	return true;
}

// Call modules on each loop iteration
bool j1Hook::Update(float dt) {	

	if (somethingHit) {
		if (arrived) {
			somethingHit = false;
			thrown = false;
			throwHook.Reset();
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_H) == j1KeyState::KEY_DOWN
		&& thrown == false && App->player->GodMode == false) {
		returnHook.Reset();
		dragHookRight.Reset();
		dragHookLeft.Reset();
		colliderPosition.x = 0;
		thrown = true;
		current_animation = &throwHook;
	}

	// If the hook is thrown
	if (thrown) {
		hookCollider->type = COLLIDER_HOOK;

		// If the hook hits something
		if (somethingHit) {			
			if (!arrived) {
				if (App->player->facingRight) {
					if (App->player->position.x < objectivePosition) {
						App->player->position.x += hookSpeed;
						current_animation = &dragHookRight;
					}
				}
				else if (App->player->position.x > objectivePosition) {
					App->player->position.x -= hookSpeed;
					current_animation = &dragHookLeft;
				}
			}
		}
		// If the hook doesn't hit anything
		else {

			if (throwHook.Finished()) {
				current_animation = &returnHook;
				throwHook.Reset();
			}

			if (returnHook.Finished()) {
				thrown = false;
			}
		}

		SDL_Rect hook = current_animation->GetCurrentFrame();

		// Blitting the hook
		if (App->player->facingRight) {
			hookPosition.x = App->player->position.x + spawnPositionRight.x;
			hookPosition.y = App->player->position.y + spawnPositionRight.y;

			App->render->Blit(graphics, (int)hookPosition.x, (int)hookPosition.y, &hook, SDL_FLIP_NONE);

			// Update collider position to hook's claw position
			if (!somethingHit) {
				if (current_animation == &throwHook)
					colliderPosition.x += hookSpeed;
				else if (current_animation == &returnHook)
					colliderPosition.x -= hookSpeed;

				hookCollider->SetPos(hookPosition.x + colliderPosition.x, hookPosition.y + 5);
			}
		}
		else{
			hookPosition.x = App->player->position.x + spawnPositionLeft.x;
			hookPosition.y = App->player->position.y + spawnPositionLeft.y;

			if(current_animation == &dragHookLeft)
				App->render->Blit(graphics, (int)hookPosition.x + 5, (int)hookPosition.y, &hook, SDL_FLIP_NONE);
			else
				App->render->Blit(graphics, (int)hookPosition.x, (int)hookPosition.y, &hook, SDL_FLIP_HORIZONTAL);

			// Update collider position to hook's claw position
			if (!somethingHit) {
				if (current_animation == &throwHook)
					colliderPosition.x -= hookSpeed;
				else if (current_animation == &returnHook)
					colliderPosition.x += hookSpeed;

				hookCollider->SetPos(hookPosition.x + 57 + colliderPosition.x, hookPosition.y + 5);
			}
		}
	}
	// If the hook is not thrown
	else {
		if (App->player->facingRight) {
			hookPosition.x = App->player->position.x + spawnPositionRight.x;
			hookPosition.y = App->player->position.y + spawnPositionRight.y;

			hookCollider->SetPos(hookPosition.x, hookPosition.y + 5);
		}
		else {
			hookPosition.x = App->player->position.x + spawnPositionLeft.x;
			hookPosition.y = App->player->position.y + spawnPositionLeft.y;

			hookCollider->SetPos(hookPosition.x + 57, hookPosition.y + 5);
		}
			
		hookCollider->type = COLLIDER_NONE;
	}
	
	return true;
}

// Called before quitting
bool j1Hook::CleanUp() {

	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(graphics);

	return true;
}

// Detects collisions
void j1Hook::OnCollision(Collider* col_1, Collider* col_2) {
	if (col_1->type == COLLIDER_HOOK) {
		somethingHit = true;
		arrived = false;
		objectivePosition = col_2->rect.x;
		App->player->feetOnGround = true;
		App->player->fallingSpeed = App->player->initialFallingSpeed;
		App->player->currentJumps = App->player->initialJumps;
	}
}