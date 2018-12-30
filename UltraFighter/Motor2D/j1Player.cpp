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
#include "j1Audio.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Hud.h"

#include "Brofiler/Brofiler.h"

j1Player::j1Player(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::PLAYER)
{
	animation = NULL;
	
	idle.LoadAnimations("idle");
	move_forward.LoadAnimations("move_forward");
	move_backwards.LoadAnimations("move_backwards");
	jump.LoadAnimations("jump");
	jump_forward.LoadAnimations("jump_forward");
	crouch.LoadAnimations("crouch");
	blocking.LoadAnimations("blocking");
	l_punch.LoadAnimations("l_punch");
	m_h_punch.LoadAnimations("m_h_punch");
	forward_m_punch.LoadAnimations("forward_m_punch");
	forward_l_punch.LoadAnimations("forward_l_punch");
	forward_h_punch.LoadAnimations("forward_h_punch");
	forward_m_kick.LoadAnimations("l_m_kick");
	high_kick.LoadAnimations("h_kick");
	forward_l_kick.LoadAnimations("forward_l_kick");
	melee_m_kick.LoadAnimations("forward_m_kick");
	forward_h_kick.LoadAnimations("forward_h_kick");
	crouch_l_punch.LoadAnimations("crouch_l_punch");
	crouch_m_punch.LoadAnimations("crouch_m_punch");
	crouch_h_punch.LoadAnimations("crouch_h_punch");
	crouch_l_kick.LoadAnimations("crouch_l_kick");
	crouch_m_kick.LoadAnimations("crouch_m_kick");
	spin_kick.LoadAnimations("crouch_h_kick");
}

j1Player::~j1Player() {}

// Load assets
bool j1Player::Start() {
	
	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/ryu.png");

	// Audios are loaded
	LOG("Loading player audios");
	if (!loadedAudios) {
		deathSound = App->audio->LoadFx("audio/fx/death.wav");
		playerHurt = App->audio->LoadFx("audio/fx/playerHurt.wav");
		jumpSound = App->audio->LoadFx("audio/fx/jump.wav");
		attackSound = App->audio->LoadFx("audio/fx/attack.wav");
		lifeup = App->audio->LoadFx("audio/fx/1-up.wav");
		loadedAudios = true;
	}

	LoadPlayerProperties();

	animation = &idle;
	currentJumps = initialJumps;
	
	lives = 2;

	// Setting player position
	position.x = initialPosition.x;
	position.y = initialPosition.y;

	collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y, 35, 85 }, COLLIDER_PLAYER, App->entity);
	
	attackCollider = App->collisions->AddCollider({ (int)position.x + rightAttackSpawnPos, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_NONE, App->entity);

	hud = new j1Hud();
	hud->Start();

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

	if (player_start)
	{
		// ---------------------------------------------------------------------------------------------------------------------
		// CONTROL OF THE PLAYER
		// ---------------------------------------------------------------------------------------------------------------------
							
		// Idle
		if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
			&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
			&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE
			&& attacking == false)
			animation = &idle;

		// Direction controls	
		if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && attacking == false && crouching == false) {			
			position.x += horizontalSpeed * dt;
			animation = &move_forward;					
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && attacking == false && crouching == false) {
			position.x -= horizontalSpeed * dt;
			animation = &move_backwards;
		}

		// Jump controls
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_DOWN && jumping == false) {
			jumping = true;
			verticalSpeed = initialVerticalSpeed;
			currentJumps++;
		}

		if (jumping) {
			position.y += verticalSpeed * dt;
			verticalSpeed += verticalAcceleration * dt;

			// While the player is falling
			if (!attacking) animation = &jump;
		}		

		// Crouch management
		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT && attacking == false) {
			animation = &crouch;
			crouching = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_UP) {
			crouch.Reset();
			crouching = false;
		}

		// Punch control
		if (App->input->GetKey(SDL_SCANCODE_O) == j1KeyState::KEY_DOWN 
			&& attacking == false && kicking == false && jumping == false) {
			attacking = true;
			punching = true;
			App->audio->PlayFx(attackSound);
			attackCollider->type = COLLIDER_ATTACK;

			if (crouching) 
				animation = &crouch_m_punch;
			else
				animation = &m_h_punch;
		}	
		// Kick control
		if (App->input->GetKey(SDL_SCANCODE_P) == j1KeyState::KEY_DOWN 
			&& attacking == false && punching == false && jumping == false) {
			attacking = true;
			kicking = true;
			App->audio->PlayFx(attackSound);
			attackCollider->type = COLLIDER_ATTACK;

			if (crouching) {
				if(App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT)
					animation = &spin_kick;
				else
					animation = &crouch_m_kick;
			}
			else {
				if (0 /*asdfasdf*/)
					animation = &forward_l_kick;
				else if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT)
					animation = &high_kick;
				else
					animation = &forward_m_kick;
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

		if (!attacking) 
			Draw(r);
		else if (animation == &m_h_punch)
			Draw(r, false, 0, -12);
		else if (animation == &crouch_m_punch)
			Draw(r, false, 0, -21);
		else if (animation == &forward_m_kick)
			Draw(r, false, 0, -18);
		else if (animation == &forward_l_kick)
			Draw(r, false, 0, -18);
		else if (animation == &crouch_m_kick)
			Draw(r, false, 0, -21);
		else if (animation == &spin_kick)
			Draw(r, false, 0, -21);
		else if (animation == &high_kick)
			Draw(r, false, 0, -21);

		// Punch management
		if (crouch_m_punch.Finished() || m_h_punch.Finished()) {

			attackCollider->type = COLLIDER_NONE;

			crouch_m_punch.Reset();
			m_h_punch.Reset();
			attacking = false;
			punching = false;
		}
		// Kick management
		else 
		if (crouch_m_kick.Finished() || forward_h_kick.Finished() || forward_m_kick.Finished() 
			|| high_kick.Finished() || spin_kick.Finished()) {

			attackCollider->type = COLLIDER_NONE;

			crouch_m_kick.Reset();
			forward_l_kick.Reset();
			forward_m_kick.Reset();
			spin_kick.Reset();
			high_kick.Reset();
			attacking = false;
			kicking = false;
		}
		else if (attackCollider != nullptr)
			attackCollider->SetPos((int)position.x + rightAttackSpawnPos, (int)position.y + margin.y);

		hud->Update(dt);

		// We update the camera to followe the player every frame
		UpdateCameraPosition();

		return true;		
	}
}

// Call modules after each loop iteration
bool j1Player::PostUpdate() {

	BROFILER_CATEGORY("PlayerPostUpdate", Profiler::Color::Yellow)

	return true;
}

// Load game state
bool j1Player::Load(pugi::xml_node& data) {

	position.x = data.child("player").child("position").attribute("x").as_int();
	position.y = data.child("player").child("position").attribute("y").as_int();

	lives = data.child("player").child("lives").attribute("value").as_uint();

	if (hud)
		hud->Load(data);

	return true;
}

// Save game state
bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	pugi::xml_node godmode = data.append_child("godmode");

	pugi::xml_node life = data.append_child("lives");
	life.append_attribute("value") = lives;

	if (hud)
		hud->Save(data.append_child("hud"));

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
	
	if (hud)
		hud->CleanUp();

	RELEASE(hud);

	return true;
}

void j1Player::UpdateCameraPosition()
{
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
			if (collider->rect.x + collider->rect.w >= col_2->rect.x + colisionMargin
				&& collider->rect.x + colisionMargin < col_2->rect.x + col_2->rect.w) {				
				//If the collision is with the ground
				if (collider->rect.y + collider->rect.h >= col_2->rect.y
					&& collider->rect.y < col_2->rect.y) {

					position.y = col_2->rect.y - collider->rect.h;

					jump.Reset();
					jumping = false;
					verticalSpeed = initialVerticalSpeed;
					fallingSpeed = initialFallingSpeed;
				}				
			}			
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
	initialFallingSpeed = speed.child("physics").attribute("initialFalling").as_float();
	fallingSpeed = speed.child("physics").attribute("falling").as_float();
	verticalAcceleration = speed.child("physics").attribute("acceleration").as_float();
	initialJumps = speed.child("physics").attribute("jumpNumber").as_uint();
	maxJumps = speed.child("physics").attribute("maxJumps").as_uint();

	cameraLimit = config.child("scene1").child("camera").attribute("cameraLimit").as_int();
	playerLimit = config.child("scene1").child("camera").attribute("playerLimit").as_int();

	deathByFallColliderHeight = player.child("deathByFallCollider").attribute("h").as_uint();
}