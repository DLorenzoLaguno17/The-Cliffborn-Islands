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

#include "Brofiler/Brofiler.h"

j1Player::j1Player(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::PLAYER)
{
	animation = NULL;
	
	idle.LoadAnimations("idle");
	run.LoadAnimations("run");
	jump.LoadAnimations("jump");
	fall.LoadAnimations("fall");
	godmode.LoadAnimations("godmode");
	attackRight.LoadAnimations("attackRight");
	attackLeft.LoadAnimations("attackLeft");
}

j1Player::~j1Player() {}

// Load assets
bool j1Player::Start() {
	
	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/character.png");

	LoadPlayerProperties();

	animation = &idle;
	currentJumps = initialJumps;
	
	// Setting player position
	position.x = initialPosition.x;
	position.y = initialPosition.y;

	collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y}, COLLIDER_PLAYER, App->entity);

	player_start = true;
	return true;
}

//Call modules before each loop iteration
bool j1Player::PreUpdate() {

	BROFILER_CATEGORY("PlayerPreUpdate", Profiler::Color::Orange)

	return true;
}

// Call modules on each loop iteration
bool j1Player::Update(float dt) {

	BROFILER_CATEGORY("PlayerUpdate", Profiler::Color::LightSeaGreen)

	// ---------------------------------------------------------------------------------------------------------------------
	// CONTROL OF THE PLAYER
	// ---------------------------------------------------------------------------------------------------------------------
	
	if (player_start)
	{
		// GodMode controls
		if (GodMode) {

			animation = &godmode;

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
				animation = &idle;

			// Direction controls	
			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && attacking == false) {
				if (wallInFront == false && dead == false && App->entity->hook->thrown == false) {
					position.x += horizontalSpeed;
					facingRight = true;
					animation = &run;
				}
				else if (dead == true) {
					facingRight = true;
					animation = &idle;
				}
				else
					animation = &idle;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && attacking == false) {
			if (wallBehind == false && dead == false && App->entity->hook->thrown == false) {
					position.x -= horizontalSpeed;
					facingRight = false;
					animation = &run;
				}
				else if (dead == true) {
					facingRight = false;
					animation = &idle;
				}
				else
					animation = &idle;
			}

			// The player falls if he has no ground
			if (feetOnGround == false && jumping == false) {

				freefall = true;
				position.y += fallingSpeed;
				fallingSpeed += verticalAcceleration;

				if (!attacking)
					animation = &fall;
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

			if (jumping == true) {
				// If the player touches a wall collider
				if (feetOnGround) {

					animation = &idle;
					jumping = false;
				}
				else {

					if (wallAbove == false && App->entity->hook->somethingHit == false) {
						position.y += verticalSpeed;
						verticalSpeed += verticalAcceleration;
					}

					// While the player is falling
					if (!attacking) {
						if (verticalSpeed <= 0) {
							animation = &jump;
						}
						else if (verticalSpeed > 0) {
							animation = &fall;
						}
					}
				}
			}
		}

		// Attack control
		if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
			attacking = true;

			if (facingRight)
				animation = &attackRight;
			else
				animation = &attackLeft;
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
				collider->type = COLLIDER_NONE;
				animation = &godmode;

			}
			else if (GodMode == false)
			{
				collider->type = COLLIDER_PLAYER;
			}
		}
	}

	// Restarting the level in case the player dies
	if (dead) {
		if (App->fade->IsFading() == 0)
		{
			fallingSpeed = initialFallingSpeed;
			position.x = initialPosition.x;
			position.y = initialPosition.y;
			App->render->camera.x = App->render->initialCameraX;
			App->render->camera.y = App->render->initialCameraY;
			jumping = false;
			currentJumps = initialJumps;
			facingRight = true;

			dead = false;

		}
	}

	// Update collider position to player position
	if (collider != nullptr)
		collider->SetPos(position.x + margin.x, position.y + margin.y);

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	// Blitting the player
	if (!attacking) {
		if (facingRight)
			Draw();
		else
			Draw(0, 0, true);
	}
	else 
		Draw(0, -2);

	return true;
}

// Call modules after each loop iteration
bool j1Player::PostUpdate() {

	BROFILER_CATEGORY("PlayerPostUpdate", Profiler::Color::Yellow)

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

	position.x = data.child("player").child("position").attribute("x").as_int();
	position.y = data.child("player").child("position").attribute("y").as_int();

	GodMode = data.child("player").child("godmode").attribute("value").as_bool();

	loading = true;

	if (GodMode == true)
	{
		collider->type = COLLIDER_NONE;
		animation = &godmode;
	}
	else if (GodMode == false)
	{
		collider->type = COLLIDER_PLAYER;
	}

	return true;
}

// Save game state
bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node playerPosition = data.child("player").append_child("position");

	playerPosition.append_attribute("x") = position.x;
	playerPosition.append_attribute("y") = position.y;

	pugi::xml_node godmode = data.child("player").append_child("godmode");

	godmode.append_attribute("value") = GodMode;

	return true;
}

// Called before quitting
bool j1Player::CleanUp() {
	
	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(sprites);
	if (collider != nullptr) 
		collider->to_delete = true;
	
	return true;
}

// Detects collisions
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_1->type == COLLIDER_PLAYER || col_1->type == COLLIDER_NONE)
	{
		if (col_2->type == COLLIDER_WALL) {
			if (collider->rect.y + collider->rect.h >= col_2->rect.y + colisionMargin
				&& collider->rect.y <= col_2->rect.y + col_2->rect.h) {
				//If the collision is with a wall in front
				if (collider->rect.x + collider->rect.w >= col_2->rect.x
					&& collider->rect.x <= col_2->rect.x) {

					wallInFront = true;
					App->entity->hook->arrived = true;
				}
				else
					//If the collision is with a wall behind
					if (collider->rect.x <= col_2->rect.x + col_2->rect.w
						&& collider->rect.x + collider->rect.w >= col_2->rect.x + col_2->rect.w) {

						wallBehind = true;
						App->entity->hook->arrived = true;
					}
			}

			if (collider->rect.x + collider->rect.w >= col_2->rect.x + colisionMargin
				&& collider->rect.x + 1 < col_2->rect.x + col_2->rect.w) {
				//If the collision is with the "ceiling"
				if (collider->rect.y <= col_2->rect.y + col_2->rect.h
					&& collider->rect.y + (collider->rect.h / 2) > col_2->rect.y + col_2->rect.h
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

						if (collider->rect.y + collider->rect.h >= col_2->rect.y
							&& collider->rect.y < col_2->rect.y + col_2->rect.h) {

							position.y = col_2->rect.y - collider->rect.h;
							feetOnGround = true;
							jumping = false;
							freefall = false;
							verticalSpeed = initialVerticalSpeed;
							fallingSpeed = initialFallingSpeed;
							currentJumps = initialJumps;
						}
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
		if (col_2->type == COLLIDER_DEATH || col_2->type == COLLIDER_ENEMY)
		{
 			App->fade->FadeToBlack(App->scene1, App->scene1);
			dead = true;
		}
	}
};

void j1Player::LoadPlayerProperties() {

	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node player;
	player = config.child("player");

	// Copying the position of the player
	initialPosition.x = player.child("position").attribute("x").as_int();
	initialPosition.y = player.child("position").attribute("y").as_int();

	// Copying the size of the player
	playerSize.x = player.child("size").attribute("width").as_int();
	playerSize.y = player.child("size").attribute("height").as_int();
	margin.x = player.child("margin").attribute("x").as_int();
	margin.y = player.child("margin").attribute("y").as_int();
	colisionMargin = player.child("margin").attribute("colisionMargin").as_uint();

	// Copying values of the speed
	pugi::xml_node speed = player.child("speed");

	initialVerticalSpeed = speed.child("movement").attribute("initialVertical").as_float();
	verticalSpeed = speed.child("movement").attribute("vertical").as_float();
	horizontalSpeed = speed.child("movement").attribute("horizontal").as_float();
	godModeSpeed = speed.child("movement").attribute("godmode").as_float();
	initialFallingSpeed = speed.child("physics").attribute("initialFalling").as_float();
	fallingSpeed = speed.child("physics").attribute("falling").as_float();
	verticalAcceleration = speed.child("physics").attribute("acceleration").as_float();
	initialJumps = speed.child("physics").attribute("jumpNumber").as_uint();
}