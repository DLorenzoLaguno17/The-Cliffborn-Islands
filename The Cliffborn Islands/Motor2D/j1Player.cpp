#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Scene1.h"

j1Player::j1Player() : j1Module()
{
	current_animation = NULL;
	
	idle_right.LoadAnimations("idle_right");
	idle_left.LoadAnimations("idle_left");
	run_right.LoadAnimations("run_right");
	run_left.LoadAnimations("run_left");
	jump_right.LoadAnimations("jump_right");
	jump_left.LoadAnimations("jump_left");
	fall_right.LoadAnimations("fall_right");
	fall_left.LoadAnimations("fall_left");	

	name.create("player");
}

j1Player::~j1Player() {}

// Reading from config file
bool j1Player::Awake(pugi::xml_node& config) {
	
	// Copying the position of the player
	initialPosition.x = config.child("position").attribute("x").as_int();
	initialPosition.y = config.child("position").attribute("y").as_int();

	// Copying values of the speed
	pugi::xml_node speed = config.child("speed");

	initialVerticalSpeed = speed.child("movement").attribute("initialVertical").as_float();
	verticalSpeed = speed.child("movement").attribute("vertical").as_float();
	horizontalSpeed = speed.child("movement").attribute("horizontal").as_float();
	initialFallingSpeed = speed.child("physics").attribute("initialFalling").as_float();
	fallingSpeed = speed.child("physics").attribute("falling").as_float();
	verticalAcceleration = speed.child("physics").attribute("acceleration").as_float();

	return true;
}

// Load assets
bool j1Player::Start() {
	
	// Textures are loaded
	LOG("Loading player textures");
	graphics = App->tex->Load("textures/character/character.png");

	current_animation = &idle_right;
	
	// Setting player position
	position.x = initialPosition.x;
	position.y = initialPosition.y;

	player = App->collisions->AddCollider({ (int)position.x, (int)position.y, 22, 25 }, COLLIDER_PLAYER, this);

	return true;
}

//Call modules before each loop iteration
bool j1Player::PreUpdate() {

	return true;
}

// Call modules on each loop iteration
bool j1Player::Update(float dt) {

	// ---------------------------------------------------------------------------------------------------------------------
	// CONTROL OF THE PLAYER
	// ---------------------------------------------------------------------------------------------------------------------
	
	// Idle
	if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE) {
		if (lastDirection == lastDirection::RIGHT)
			current_animation = &idle_right;
		else
			current_animation = &idle_left;
	}

	// Direction controls	
	if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
		if (wallInFront == false && dead == false) {
			position.x += horizontalSpeed;
			lastDirection = lastDirection::RIGHT;
			current_animation = &run_right;
		}
		else if (dead == true) {
			lastDirection = lastDirection::RIGHT;
			current_animation = &idle_right;
		}
		else
			current_animation = &idle_right;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
		if (wallBehind == false && dead == false) {
			position.x -= horizontalSpeed;
			lastDirection = lastDirection::LEFT;
			current_animation = &run_left;
		}
		else if (dead == true) {
			lastDirection = lastDirection::LEFT;
			current_animation = &idle_left;
		}
		else
			current_animation = &idle_left;
	}

	// The player falls if he has no ground
	if (feetOnGround == false && jumping == false) {

		position.y += fallingSpeed;
		fallingSpeed += verticalAcceleration;
		falling = true;

		if (lastDirection == lastDirection::RIGHT)
			current_animation = &fall_right;
		else
			current_animation = &fall_left;
	}

	// Jump controls
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_DOWN && jumps <= 1 && falling == false) {
			jumping = true;
			verticalSpeed = initialVerticalSpeed;
			jumps++;		
	}

	// Reseting the jump every frame
	feetOnGround = false;

	// Resetting the movement
	wallInFront = false;
	wallBehind = false;

	if (jumping == true && dead == false) {
		// If the player touches a wall collider
		if (feetOnGround) {
			if (lastDirection == lastDirection::RIGHT)
				current_animation = &idle_right;
			else
				current_animation = &idle_left;
		}
		else {

			position.y += verticalSpeed;
			verticalSpeed += verticalAcceleration;

			// If the player is going right
			if (lastDirection == lastDirection::RIGHT) {
				if (verticalSpeed <= 0) {
					current_animation = &jump_right;
				}
				else if (verticalSpeed > 0) {
					current_animation = &fall_right;
				}
			}
			// If the player is going left
			if (lastDirection == lastDirection::LEFT) {
				if (verticalSpeed <= 0) {
					current_animation = &jump_left;
				}
				else if (verticalSpeed > 0) {
					current_animation = &fall_left;
				}
			}
		}
	}

	// God mode
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		GodMode = !GodMode;

		if (GodMode == true)
		{
			player->type = COLLIDER_NONE;
		}
		else if (GodMode == false)
		{
			player->type = COLLIDER_PLAYER;
		}
	}

	// Update collider position to player position
	player->SetPos(position.x, position.y);

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	SDL_Rect character = current_animation->GetCurrentFrame();

	// Blitting the player
	App->render->Blit(graphics, (int)position.x, (int)position.y, &character);

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

	loading = true;

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

// Detects collision with a wall. If so, the player cannot go further
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (((col_1->type == COLLIDER_PLAYER || col_1->type == COLLIDER_NONE) && col_2->type == COLLIDER_WALL || col_2->type == COLLIDER_DEATH || col_2->type == COLLIDER_WIN)
		|| ((col_2->type == COLLIDER_PLAYER || col_2->type == COLLIDER_PLAYER) || col_1->type == COLLIDER_WALL || col_1->type == COLLIDER_DEATH || col_1->type == COLLIDER_WIN))
	{
		//If the collision is with a wall behind
		if (col_1->type == COLLIDER_WALL) {

			if (position.x < col_1->rect.x + col_1->rect.w
				&& position.x - col_1->rect.x > 0
				&& position.y + current_animation->GetCurrentFrame().h > col_1->rect.y + 10) {
				wallBehind = true;
			}
			//If the collision is with a wall in front
			else if (position.x + current_animation->GetCurrentFrame().w > col_1->rect.x
				&& position.y + current_animation->GetCurrentFrame().h > col_1->rect.y + 10) {
				wallInFront = true;
			}
		}
		else if (col_2->type == COLLIDER_WALL) {
			
			if (position.x < col_2->rect.x + col_2->rect.w
				&& position.x - col_2->rect.x > 0
				&& position.y + current_animation->GetCurrentFrame().h > col_2->rect.y + 10) {
				wallBehind = true;
			}
			else if (position.x + current_animation->GetCurrentFrame().w > col_2->rect.x
				&& position.y + current_animation->GetCurrentFrame().h > col_2->rect.y + 10) {
				wallInFront = true;
			}
		}		

		feetOnGround = true;
		jumping = false;
		falling = false;
		verticalSpeed = initialVerticalSpeed;
		fallingSpeed = initialFallingSpeed;
		jumps = 0;

		//If the collision is with the ground
		/*if (col_1->type == COLLIDER_WALL) {
			feetOnGround = true;
			jumping = false;
			verticalSpeed = initialVerticalSpeed;
			fallingSpeed = 0.0f;

			if (position.y + current_animation->GetCurrentFrame().h > col_1->rect.y 
				&& wallInFront == false && wallBehind == false && loading == false) {
				position.y = col_1->rect.y - current_animation->GetCurrentFrame().h + 2;
			}
		}
		else if(col_2->type == COLLIDER_WALL) {
			feetOnGround = true;
			jumping = false;
			verticalSpeed = initialVerticalSpeed;
			fallingSpeed = 0.0f;

			if (position.y + current_animation->GetCurrentFrame().h > col_2->rect.y 
				&& wallInFront == false && wallBehind == false && loading == false) {
				position.y = col_2->rect.y - current_animation->GetCurrentFrame().h + 2;
			}			
		}*/

		//If the player collides with death_colliders
		if (col_1->type == COLLIDER_DEATH || col_2->type == COLLIDER_DEATH)
		{
			App->fade->FadeToBlack(App->scene1, App->scene1);
			fallingSpeed = initialFallingSpeed;
			dead = true;
		
			if (App->fade->IsFading() == 0)
			{
				position.x = initialPosition.x;
				position.y = initialPosition.y;
				App->render->camera.x = App->render->initialCameraX;
				App->render->camera.y = App->render->initialCameraY;
				dead = false;
			}
		}

		if (col_1->type == COLLIDER_WIN || col_2->type == COLLIDER_WIN)
		{
			App->fade->FadeToBlack(App->scene1, App->scene1);
			fallingSpeed = initialFallingSpeed;
			dead = true;

			if (App->fade->IsFading() == 0)
			{
				position.x = initialPosition.x;
				position.y = initialPosition.y;
				App->render->camera.x = App->render->initialCameraX;
				App->render->camera.y = App->render->initialCameraY;
				dead = false;
			}
			
		}

		//This is only to ensure the player is loaded in the correct position, once it is, we don't need this anymore
		loading = false;
	}
};