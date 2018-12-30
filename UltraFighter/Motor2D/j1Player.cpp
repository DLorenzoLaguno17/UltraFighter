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

j1Player::j1Player(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::PLAYER1)
{
	animation = NULL;
	
	idle.LoadAnimations("idle");
	move_forward.LoadAnimations("move_forward");
	move_backwards.LoadAnimations("move_backwards");
	jump.LoadAnimations("jump");
	jump_forward.LoadAnimations("jump_forward");
	crouch.LoadAnimations("crouch");
	blocking.LoadAnimations("blocking");
	short_punch.LoadAnimations("l_punch");
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
	receive_damage_idle.LoadAnimations("damage_idle");
	receive_damage_crouch.LoadAnimations("damage_crouch");
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

	// Setting player position
	position.x = 3;
	position.y = 160;

	collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y, 35, 85 }, COLLIDER_PLAYER1, App->entity);

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

	if (player_start && receivedDmg == false)
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
		if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && attacking == false && crouching == false ) {
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
		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT && attacking == false && jumping == false) {
			animation = &crouch;
			crouching = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_UP) {
			crouch.Reset();
			crouching = false;
		}

		// Punch control
		if (App->input->GetKey(SDL_SCANCODE_T) == j1KeyState::KEY_DOWN
			&& attacking == false && kicking == false && jumping == false) {
			attacking = true;
			punching = true;
			App->audio->PlayFx(attackSound);

			if (crouching) {
				animation = &crouch_m_punch;
				attackCollider = App->collisions->AddCollider({ (int)position.x + 37, (int)position.y + 40, 26, 15 }, COLLIDER_ATTACK1, App->entity);
			}
			else {
				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
					animation = &m_h_punch;
					attackCollider = App->collisions->AddCollider({ (int)position.x + 37, (int)position.y + 17, 35, 15 }, COLLIDER_ATTACK1, App->entity);
				}
				else if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
					animation = &forward_h_punch;
					attackCollider = App->collisions->AddCollider({ (int)position.x + 45, (int)position.y - 10, 15, 35 }, COLLIDER_ATTACK1, App->entity);
				}
				else {
					animation = &short_punch;
					attackCollider = App->collisions->AddCollider({ (int)position.x + 37, (int)position.y + 17, 25, 15 }, COLLIDER_ATTACK1, App->entity);
				}
			}
		}

		// Kick control
		if (App->input->GetKey(SDL_SCANCODE_Y) == j1KeyState::KEY_DOWN 
			&& attacking == false && punching == false && jumping == false) {
			attacking = true;
			kicking = true;
			App->audio->PlayFx(attackSound);

			if (crouching) {
				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
					animation = &crouch_m_kick;
					attackCollider = App->collisions->AddCollider({ (int)position.x + 37, (int)position.y + 75, 52, 15 }, COLLIDER_ATTACK1, App->entity);
				}
				else if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
					animation = &spin_kick;
					attackCollider = App->collisions->AddCollider({ (int)position.x + 37, (int)position.y + 75, 35, 15 }, COLLIDER_ATTACK1, App->entity);
				}
				else {
					animation = &crouch_l_kick;
					attackCollider = App->collisions->AddCollider({ (int)position.x + 37, (int)position.y + 75, 35, 15 }, COLLIDER_ATTACK1, App->entity);
				}
			}
			else {
				if (0 /*asdfasdf*/) {
					animation = &forward_l_kick;
				}
				else if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
					animation = &high_kick;
					attackCollider = App->collisions->AddCollider({ (int)position.x + 50, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_ATTACK1, App->entity);
				}
				else {
					animation = &forward_m_kick; 
					attackCollider = App->collisions->AddCollider({ (int)position.x + 50, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_ATTACK1, App->entity);
				}
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
		else if (animation == &short_punch)
			Draw(r, false, 0, -12);
		else if (animation == &forward_h_punch)
			Draw(r, false, 0, -12);
		else if (animation == &crouch_m_punch)
			Draw(r, false, 0, -21);
		else if (animation == &forward_m_kick)
			Draw(r, false, 0, -18);
		else if (animation == &forward_l_kick)
			Draw(r, false, 0, -18);
		else if (animation == &crouch_m_kick)
			Draw(r, false, 0, -21);
		else if (animation == &crouch_l_kick)
			Draw(r, false, 0, -21);
		else if (animation == &spin_kick)
			Draw(r, false, 0, -21);
		else if (animation == &high_kick)
			Draw(r, false, 0, -21);

		// Punch management
		if (crouch_m_punch.Finished() || m_h_punch.Finished() || short_punch.Finished()
			|| forward_h_punch.Finished()) {

			crouch_m_punch.Reset();
			m_h_punch.Reset();
			short_punch.Reset();
			attacking = false;
			forward_h_punch.Reset();
			punching = false;
			attackCollider->to_delete = true;

			if (attackCollider != nullptr)
				attackCollider->to_delete = true;
		}
		// Kick management
		else
		if (crouch_m_kick.Finished() || forward_h_kick.Finished() || forward_m_kick.Finished()
			|| high_kick.Finished() || spin_kick.Finished() || crouch_l_kick.Finished()) {

			crouch_m_kick.Reset();
			crouch_l_kick.Reset();
			forward_l_kick.Reset();
			forward_m_kick.Reset();
			spin_kick.Reset();
			high_kick.Reset();
			attacking = false;
			kicking = false;

			if(attackCollider != nullptr)
				attackCollider->to_delete = true;
		}

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
	if (position.y > 160) {
		position.y = 160;
		jump.Reset();
		jumping = false;
		verticalSpeed = initialVerticalSpeed;
		fallingSpeed = initialFallingSpeed;
	}

	// Limit player X position
	if (position.x > 290)
		position.x = 290;

	// To force the player to go forward at the start of the level
	if (position.x < 0)
		position.x = 0;

}
// Detects collisions
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_1->type == COLLIDER_PLAYER1)
	{		
						
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
	
	// Copying values of the speed
	pugi::xml_node speed = player.child("speed");

	initialVerticalSpeed = speed.child("movement").attribute("initialVertical").as_float();
	verticalSpeed = speed.child("movement").attribute("vertical").as_float();
	horizontalSpeed = speed.child("movement").attribute("horizontal").as_float();
	initialFallingSpeed = speed.child("physics").attribute("initialFalling").as_float();
	fallingSpeed = speed.child("physics").attribute("falling").as_float();
	verticalAcceleration = speed.child("physics").attribute("acceleration").as_float();
	initialJumps = speed.child("physics").attribute("jumpNumber").as_uint();
}