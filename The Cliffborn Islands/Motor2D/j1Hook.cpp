#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Audio.h"
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
	leftMargin = config.attribute("leftMargin").as_int();

	// Copying collider values
	hookSize.x = config.child("size").attribute("width").as_int();
	hookSize.y = config.child("size").attribute("height").as_int();
	initialColliderPosition = config.child("collider").attribute("colliderPosition").as_int();
	heightMargin = config.child("collider").attribute("heightMargin").as_int();

	// Copying the spawning positions
	spawnPositionRight.x = config.child("spawnPositionRight").attribute("x").as_int();
	spawnPositionRight.y = config.child("spawnPositionRight").attribute("y").as_int();
	spawnPositionLeft.x = config.child("spawnPositionLeft").attribute("x").as_int();
	spawnPositionLeft.y = config.child("spawnPositionLeft").attribute("y").as_int();

	return true;
}

// Load assets
bool j1Hook::Start() {
	
	// Textures are loaded
	LOG("Loading player textures");
	graphics = App->tex->Load("textures/character/hook.png");

	// Audios are loaded
	chain = App->audio->LoadFx("audio/fx/chain.wav");

	current_animation = &throwHook;
	
	hookCollider = App->collisions->AddCollider({NULL, NULL, hookSize.x, hookSize.y}, COLLIDER_HOOK, this);

	return true;
}

// Call modules on each loop iteration
bool j1Hook::Update(float dt) {	

	// We reset the values if the player has arrived to it's hooked destination
	if (somethingHit) {
		if (arrived || dragHookLeft.Finished() || dragHookRight.Finished()) {
			somethingHit = false;
			thrown = false;
			throwHook.Reset();
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_O) == j1KeyState::KEY_DOWN
		&& thrown == false && App->player->GodMode == false) {
		returnHook.Reset();
		dragHookRight.Reset();
		dragHookLeft.Reset();
		colliderPosition = initialColliderPosition;
		thrown = true;
		current_animation = &throwHook;
		App->audio->PlayFx(chain);
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

			// Update collider position to hook's claw position with the player facing right
			if (!somethingHit) {
				if (current_animation == &throwHook)
					colliderPosition += hookSpeed;
				else if (current_animation == &returnHook)
					colliderPosition -= hookSpeed;

				hookCollider->SetPos(hookPosition.x + colliderPosition, hookPosition.y + heightMargin);
			}
		}
		else{
			hookPosition.x = App->player->position.x + spawnPositionLeft.x;
			hookPosition.y = App->player->position.y + spawnPositionLeft.y;

			if(current_animation == &dragHookLeft)
				App->render->Blit(graphics, (int)hookPosition.x + 5, (int)hookPosition.y, &hook, SDL_FLIP_NONE);
			else
				App->render->Blit(graphics, (int)hookPosition.x, (int)hookPosition.y, &hook, SDL_FLIP_HORIZONTAL);

			// Update collider position to hook's claw position with the player facing left
			if (!somethingHit) {
				if (current_animation == &throwHook)
					colliderPosition -= hookSpeed;
				else if (current_animation == &returnHook)
					colliderPosition += hookSpeed;

				hookCollider->SetPos(hookPosition.x + leftMargin + colliderPosition, hookPosition.y + heightMargin);
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

			hookCollider->SetPos(hookPosition.x + leftMargin, hookPosition.y + heightMargin);
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
	if (col_1->type == COLLIDER_HOOK && current_animation != &returnHook) {
		
		// First we check if the collision is perpendicular
		if (col_1->rect.y + col_1->rect.h >= col_2->rect.y
			&& col_1->rect.y <= col_2->rect.y + col_2->rect.h){

				// If the hook hits right
			if ((App->player->facingRight == true
				&& col_1->rect.x + col_1->rect.w >= col_2->rect.x
				&& col_1->rect.x <= col_2->rect.x) ||
				// If the hook hits left
				(App->player->facingRight == false &&
				col_1->rect.x <= col_2->rect.x + col_2->rect.w
				&& col_1->rect.x + col_1->rect.w >= col_2->rect.x + col_2->rect.w)) {

				somethingHit = true;
				arrived = false;
				App->player->feetOnGround = true;
				App->player->fallingSpeed = App->player->initialFallingSpeed;
				App->player->currentJumps = App->player->initialJumps;

				if (App->player->facingRight)
					objectivePosition = col_2->rect.x - App->player->player->rect.w - App->player->colisionMargin;
				else
					objectivePosition = col_2->rect.x + col_2->rect.w - App->player->colisionMargin;
			}
		}
	}
}