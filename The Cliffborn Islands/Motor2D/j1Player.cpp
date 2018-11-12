#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Hook.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1Harpy.h"

j1Player::j1Player() : j1Module()
{
	current_animation = NULL;
	
	idle.LoadAnimations("idle");
	run.LoadAnimations("run");
	jump.LoadAnimations("jump");
	fall.LoadAnimations("fall");
	godmode.LoadAnimations("godmode");
	attackRight.LoadAnimations("attackRight");
	attackLeft.LoadAnimations("attackLeft");

	name.create("player");
}

j1Player::~j1Player() {}

// Reading from config file
bool j1Player::Awake(pugi::xml_node& config) {
	
	// Copying the position of the player
	initialPosition.x = config.child("position").attribute("x").as_int();
	initialPosition.y = config.child("position").attribute("y").as_int();

	// Copying the size of the player
	playerSize.x = config.child("size").attribute("width").as_int();
	playerSize.y = config.child("size").attribute("height").as_int();
	margin.x = config.child("margin").attribute("x").as_int();
	margin.y = config.child("margin").attribute("y").as_int();
	colisionMargin = config.child("margin").attribute("colisionMargin").as_uint();

	// Copying values of the speed
	pugi::xml_node speed = config.child("speed");

	initialVerticalSpeed = speed.child("movement").attribute("initialVertical").as_float();
	verticalSpeed = speed.child("movement").attribute("vertical").as_float();
	horizontalSpeed = speed.child("movement").attribute("horizontal").as_float();
	godModeSpeed = speed.child("movement").attribute("godmode").as_float();
	initialFallingSpeed = speed.child("physics").attribute("initialFalling").as_float();
	fallingSpeed = speed.child("physics").attribute("falling").as_float();
	verticalAcceleration = speed.child("physics").attribute("acceleration").as_float();
	initialJumps = speed.child("physics").attribute("jumpNumber").as_uint();

	return true;
}

// Load assets
bool j1Player::Start() {
	
	// Textures are loaded
	LOG("Loading player textures");
	graphics = App->tex->Load("textures/character/character.png");

	current_animation = &idle;
	currentJumps = initialJumps;
	
	// Setting player position
	position.x = initialPosition.x;
	position.y = initialPosition.y;

	player = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y}, COLLIDER_PLAYER, this);

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
	
	// GodMode controls
	if (GodMode) {

		current_animation = &godmode;

		if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
			position.x += godModeSpeed;
			facingRight = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
			position.x -= godModeSpeed;
			facingRight = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
			position.y -= godModeSpeed;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT) {
			position.y += godModeSpeed;
		}
	}
	else {
		// Idle
		if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
			&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
			&& attacking == false) 
			current_animation = &idle;		

		// Direction controls	
		if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && attacking == false) {
			if (wallInFront == false && dead == false && App->hook->thrown == false) {
				position.x += horizontalSpeed;
				facingRight = true;
				current_animation = &run;
			}
			else if (dead == true) {
				facingRight = true;
				current_animation = &idle;
			}
			else
				current_animation = &idle;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && attacking == false) {
			if (wallBehind == false && dead == false && App->hook->thrown == false) {
				position.x -= horizontalSpeed;
				facingRight = false;
				current_animation = &run;
			}
			else if (dead == true) {
				facingRight = false;
				current_animation = &idle;
			}
			else
				current_animation = &idle;			
		}

		// The player falls if he has no ground
		if (feetOnGround == false && jumping == false) {

			freefall = true;
			position.y += fallingSpeed;
			fallingSpeed += verticalAcceleration;

			if(!attacking)
				current_animation = &fall;
		}

		// Jump controls
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_DOWN) {
			if ((currentJumps == 0 && freefall == true) || (currentJumps <= 1 && freefall == false)) {
				jumping = true;
				verticalSpeed = initialVerticalSpeed;
				currentJumps++;
			}
		}

		// Reseting the jump every frame
		feetOnGround = false;

		if (jumping == true && dead == false) {
			// If the player touches a wall collider
			if (feetOnGround) {

				current_animation = &idle;
				jumping = false;
			}
			else {

				if (wallAbove == false && App->hook->somethingHit == false) {
					position.y += verticalSpeed;
					verticalSpeed += verticalAcceleration;
				}

				// While the player is falling
				if (!attacking) {
					if (verticalSpeed <= 0) {
						current_animation = &jump;
					}
					else if (verticalSpeed > 0) {
						current_animation = &fall;
					}
				}
			}
		}
	}

	// Attack control
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		attacking = true;

		if (facingRight)
			current_animation = &attackRight;
		else
			current_animation = &attackLeft;
	}

	if ((facingRight && attackRight.Finished())
		|| (!facingRight && attackLeft.Finished())) {

		attackLeft.Reset();
		attackRight.Reset();
		attacking = false;
	}

	// God mode
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		GodMode = !GodMode;

		if (GodMode == true)
		{
			player->type = COLLIDER_NONE;
			current_animation = &godmode;
			
		}
		else if (GodMode == false)
		{
			player->type = COLLIDER_PLAYER;
		}
	}

	// Update collider position to player position
	player->SetPos(position.x + margin.x, position.y + margin.y);

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	SDL_Rect character = current_animation->GetCurrentFrame();

	// Blitting the player
	if (!attacking) {
		if (facingRight)
			App->render->Blit(graphics, (int)position.x, (int)position.y, &character, SDL_FLIP_NONE);
		else
			App->render->Blit(graphics, (int)position.x, (int)position.y, &character, SDL_FLIP_HORIZONTAL);
	}
	else 
		App->render->Blit(graphics, (int)position.x, (int)position.y - 2, &character, SDL_FLIP_NONE);

	return true;
}

// Call modules after each loop iteration
bool j1Player::PostUpdate() {

	loading = false;

	// Resetting the jump if touched the "ceiling"
	wallAbove = false;

	// Resetting the movement
	wallInFront = false;
	wallBehind = false;

	return true;
}

// Load game state
bool j1Player::Load(pugi::xml_node& data) {

	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();

	GodMode = data.child("godmode").attribute("value").as_bool();

	loading = true;

	if (GodMode == true)
	{
		player->type = COLLIDER_NONE;
		current_animation = &godmode;
	}
	else if (GodMode == false)
	{
		player->type = COLLIDER_PLAYER;
	}

	return true;
}

// Save game state
bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node playerPosition = data.append_child("position");

	playerPosition.append_attribute("x") = position.x;
	playerPosition.append_attribute("y") = position.y;

	pugi::xml_node godmode = data.append_child("godmode");

	godmode.append_attribute("value") = GodMode;

	return true;
}

// Called before quitting
bool j1Player::CleanUp() {
	
	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(graphics);

	return true;
}

// Detects collisions
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_1->type == COLLIDER_PLAYER || col_1->type == COLLIDER_NONE)
	{
		if (player->rect.y + player->rect.h >= col_2->rect.y + colisionMargin
			&& player->rect.y <= col_2->rect.y + col_2->rect.h) {
			//If the collision is with a wall in front
			if (player->rect.x + player->rect.w >= col_2->rect.x
				&& player->rect.x <= col_2->rect.x) {

				wallInFront = true;
				App->hook->arrived = true;				
			}
			else
			//If the collision is with a wall behind
			if (player->rect.x <= col_2->rect.x + col_2->rect.w				
				&& player->rect.x + player->rect.w >= col_2->rect.x + col_2->rect.w) {

				wallBehind = true;
				App->hook->arrived = true;
			}
		}
		
		if (player->rect.x + player->rect.w >= col_2->rect.x + colisionMargin
			&& player->rect.x + 1 < col_2->rect.x + col_2->rect.w) {
			//If the collision is with the "ceiling"
			if (player->rect.y <= col_2->rect.y + col_2->rect.h
				&& player->rect.y + (player->rect.h / 2) > col_2->rect.y + col_2->rect.h				
				&& verticalSpeed < 0) {

				position.y = col_2->rect.y + col_2->rect.h;
				wallAbove = true;
				jumping = false;
				fallingSpeed = initialFallingSpeed;
				currentJumps++;
			}
			else
			//If the collision is with the ground
			if (loading == false) {

				if (player->rect.y + player->rect.h >= col_2->rect.y					
					&& player->rect.y < col_2->rect.y + col_2->rect.h) {

					position.y = col_2->rect.y - player->rect.h;
					feetOnGround = true;
					jumping = false;
					freefall = false;
					verticalSpeed = initialVerticalSpeed;
					fallingSpeed = initialFallingSpeed;
					currentJumps = initialJumps;
				}
			}
		}

		//If the player collides with win colliders
		if (col_2->type == COLLIDER_WIN)
		{
			if (App->scene1->active)
				App->scene1->ChangeScene();
			else if (App->scene2->active)
				App->scene2->ChangeScene();
		}

		//If the player collides with death colliders
		if (col_2->type == COLLIDER_DEATH)
		{
			App->fade->FadeToBlack(App->scene1, App->scene1);
			fallingSpeed = initialFallingSpeed;
			dead = true;
			jumping = false;
			currentJumps = initialJumps;
			App->harpy->position = { 250, 20 };
			App->harpy->path->Clear();

			if (App->fade->IsFading() == 0)
			{
				position.x = initialPosition.x;
				position.y = initialPosition.y;
				App->render->camera.x = App->render->initialCameraX;
				App->render->camera.y = App->render->initialCameraY;
				dead = false;
			}
		}
	}
};