#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Scene1.h"

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
	death.LoadAnimations("death");
}

j1Player::~j1Player() {}

// Load assets
bool j1Player::Start() {
	
	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/character.png");

	// Audios are loaded
	LOG("Loading player audios");
	if (!loadedAudios) {
		deathSound = App->audio->LoadFx("audio/fx/death.wav");
		playerHurt = App->audio->LoadFx("audio/fx/playerHurt.wav");
		jumpSound = App->audio->LoadFx("audio/fx/jump.wav");
		attackSound = App->audio->LoadFx("audio/fx/attack.wav");
		loadedAudios = true;
	}

	LoadPlayerProperties();

	animation = &idle;
	currentJumps = initialJumps;
	
	// Setting player position
	position.x = initialPosition.x;
	position.y = initialPosition.y;

	if (GodMode)
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y}, COLLIDER_NONE, App->entity);
	else
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_PLAYER, App->entity);
	
	player_start = true;
	return true;
}

//Call modules before each loop iteration
bool j1Player::PreUpdate() {

	BROFILER_CATEGORY("PlayerPreUpdate", Profiler::Color::Orange)

	return true;
}

// Call modules on each loop iteration
bool j1Player::Update(float dt, bool do_logic) {

	BROFILER_CATEGORY("PlayerUpdate", Profiler::Color::LightSeaGreen)

	// ---------------------------------------------------------------------------------------------------------------------
	// CONTROL OF THE PLAYER
	// ---------------------------------------------------------------------------------------------------------------------
	
	if (player_start)
	{
		// GodMode controls
		if (GodMode) {

			animation = &godmode;

			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT 
				|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) == 1
				|| App->input->gamepadP1LAxisX > 6400) {
				position.x += godModeSpeed * dt;
				facingRight = true;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT 
				|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) == 1
				|| App->input->gamepadP1LAxisX < -6400) {
				position.x -= godModeSpeed * dt;
				facingRight = false;
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT
				|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_DPAD_UP)) == 1
				|| App->input->gamepadP1LAxisY < -6400)
			{
				position.y -= godModeSpeed * dt;
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT 
				|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) == 1
				|| App->input->gamepadP1LAxisY > 6400) {
				position.y += godModeSpeed * dt;
			}
		}
		else {
			// Idle
			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
				&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
				&& attacking == false)
				animation = &idle;

			// Direction controls	
			if ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT 
				|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) == 1
				|| App->input->gamepadP1LAxisX > 6400) && attacking == false) {
				if (wallInFront == false && dead == false) {
					position.x += horizontalSpeed * dt;
					animation = &run;

					facingRight = true;
				}
				else if (dead == true) {
					facingRight = true;
					animation = &idle;
				}
				else
					animation = &idle;
			}

			if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT 
				|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) == 1
				|| App->input->gamepadP1LAxisX < -6400) && attacking == false) {
				if (wallBehind == false && dead == false) {
					position.x -= horizontalSpeed * dt;
					animation = &run;

					facingRight = false;
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
				position.y += fallingSpeed * dt;
				fallingSpeed += verticalAcceleration * dt;

				if (!attacking)
					animation = &fall;
			}

			// Jump controls
			if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_DOWN) {
				if ((currentJumps == initialJumps && freefall == true) || (currentJumps < maxJumps && freefall == false)) {
					jumping = true;
					verticalSpeed = initialVerticalSpeed;
					currentJumps++;

					if(freefall == true || (currentJumps > 1 && freefall == false))
						App->audio->PlayFx(jumpSound);
				}
			}

			// Reseting the jump every frame
			feetOnGround = false;
			if (dead && deathByFall == false)
				animation = &death;

			if (jumping == true && animation != &death) {
				// If the player touches a wall collider
				if (feetOnGround) {

					animation = &idle;
					jumping = false;
				}
				else {

					if (wallAbove == false) {
						position.y += verticalSpeed * dt;
						verticalSpeed += verticalAcceleration * dt;
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
		if ((App->input->GetKey(SDL_SCANCODE_P) == j1KeyState::KEY_DOWN || (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_X)) == 1 )
			&& attacking == false && GodMode == false && dead == false) {
			attacking = true;
			App->audio->PlayFx(attackSound);
						
			if (facingRight) {
				attackCollider = App->collisions->AddCollider({ (int)position.x + rightAttackSpawnPos, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_ATTACK, App->entity);
				animation = &attackRight;
			}
			else {
				attackCollider = App->collisions->AddCollider({ (int)position.x + leftAttackSpawnPos, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_ATTACK, App->entity);
				animation = &attackLeft;
			}
		}

		// Attack management
		if ((facingRight && attackRight.Finished())
			|| (!facingRight && attackLeft.Finished()) || dead == true) {

			if (attackCollider != nullptr)
				attackCollider->to_delete = true;

			attackLeft.Reset();
			attackRight.Reset();
			animation = &idle;
			attacking = false;
		}
		else if(attackCollider != nullptr) {
			if (facingRight)
				attackCollider->SetPos((int)position.x + rightAttackSpawnPos, (int)position.y + margin.y);
			else
				attackCollider->SetPos((int)position.x + leftAttackSpawnPos, (int)position.y + margin.y);
		}

		// God mode
		if (App->input->GetKey(SDL_SCANCODE_F10) == j1KeyState::KEY_DOWN && dead == false)
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

	if (dead) {
		// Death animation is not shown if the player dies by falling
		if (!deathByFall)
			animation = &death;

		// Restarting the level in case the player dies
		if (App->fade->IsFading() == 0)
		{
			position.x = initialPosition.x;
			position.y = initialPosition.y;
			fallingSpeed = initialFallingSpeed;
			App->render->camera.x = App->render->initialCameraX;
			App->render->camera.y = App->render->initialCameraY;
			jumping = false;
			facingRight = true;
			deathByFall = false;
			playedSound = false;

			// Resetting the animation
			death.Reset();
			attackLeft.Reset();
			attackRight.Reset();
			animation = &idle;

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
	SDL_Rect r = animation->GetCurrentFrame(dt);

	if (!attacking) {
		if (facingRight)
			Draw(r);
		else
			Draw(r, true);
	}
	else if (animation == &attackLeft || animation == &attackRight){
		if(facingRight)
			Draw(r, false, 0, attackBlittingY);
		else
			Draw(r, false, attackBlittingX, attackBlittingY);
	}

	// We update the camera to followe the player every frame
	UpdateCameraPosition();

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

	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	pugi::xml_node godmode = data.append_child("godmode");

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

	if (attackCollider != nullptr)
		attackCollider->to_delete = true;
	
	return true;
}

void j1Player::UpdateCameraPosition()
{
	if(App->render->camera.x > cameraLimit)
		App->render->camera.x = -position.x * 4 + 400;	

	//Limit X camera position
	if (App->render->camera.x > 0)
		App->render->camera.x = 0;

	//Limit player X position
	if (App->entity->player->position.x > playerLimit)
		App->entity->player->position.x = playerLimit;

	// To force the player to go forward at the start of the level
	if (App->entity->player->position.x < 0)
		App->entity->player->position.x = 0;

}
// Detects collisions
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_1->type == COLLIDER_PLAYER || col_1->type == COLLIDER_NONE)
	{
		// If the player collides with a wall
		if (col_2->type == COLLIDER_WALL) {
			if (collider->rect.y + collider->rect.h >= col_2->rect.y + colisionMargin
				&& collider->rect.y <= col_2->rect.y + col_2->rect.h) {
				//If the collision is with a wall in front
				if (collider->rect.x + collider->rect.w >= col_2->rect.x
					&& collider->rect.x <= col_2->rect.x) {

					wallInFront = true;

					if(position.x + (collider->rect.w * 3 / 4) < col_2->rect.x)
						position.x = col_2->rect.x - collider->rect.w;
				}
				else
				//If the collision is with a wall behind
				if (collider->rect.x <= col_2->rect.x + col_2->rect.w
					&& collider->rect.x + collider->rect.w >= col_2->rect.x + col_2->rect.w) {

					wallBehind = true;
					if(position.x + (collider->rect.w / 4) > col_2->rect.x + col_2->rect.w)
						position.x = col_2->rect.x + col_2->rect.w - colisionMargin;
				}
			}

			if (collider->rect.x + collider->rect.w >= col_2->rect.x + colisionMargin
				&& collider->rect.x + colisionMargin < col_2->rect.x + col_2->rect.w) {
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
						&& collider->rect.y < col_2->rect.y) {

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

		//If the player collides with death colliders
		if (col_2->type == COLLIDER_DEATH || col_2->type == COLLIDER_ENEMY)
		{
			if (col_2->rect.h < deathByFallColliderHeight)
				deathByFall = true;
			else {
				if (!playedSound) {
					App->audio->PlayFx(playerHurt);
					playedSound = true;
				}

				jumping = false;
				fallingSpeed = initialFallingSpeed;
			}
			App->entity->DestroyEntities();
			if (App->scene1->active == true)
				App->fade->FadeToBlack(App->scene1, App->scene1, 3.0f);

			dead = true;
			App->audio->PlayFx(deathSound);
			currentJumps == maxJumps;
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

	// Copying the size of the player
	playerSize.x = player.child("size").attribute("width").as_int();
	playerSize.y = player.child("size").attribute("height").as_int();
	margin.x = player.child("margin").attribute("x").as_int();
	margin.y = player.child("margin").attribute("y").as_int();
	colisionMargin = player.child("margin").attribute("colisionMargin").as_uint();

	// Copying attack values
	attackBlittingX = player.child("attack").attribute("blittingX").as_int();
	attackBlittingY = player.child("attack").attribute("blittingY").as_int();
	rightAttackSpawnPos = player.child("attack").attribute("rightColliderSpawnPos").as_int();
	leftAttackSpawnPos = player.child("attack").attribute("leftColliderSpawnPos").as_int();

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
	maxJumps = speed.child("physics").attribute("maxJumps").as_uint();

	cameraLimit = config.child("scene1").child("camera").attribute("cameraLimit").as_int();
	playerLimit = config.child("scene1").child("camera").attribute("playerLimit").as_int();

	deathByFallColliderHeight = player.child("deathByFallCollider").attribute("h").as_uint();
}