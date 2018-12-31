#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player2.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Hook.h"
#include "j1Audio.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Hud.h"
#include "j1Player.h"

#include "Brofiler/Brofiler.h"

j1Player2::j1Player2(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::PLAYER2)
{
	animation = NULL;

	idle.LoadAnimations2("idle");
	move_forward.LoadAnimations2("move_forward");
	move_backwards.LoadAnimations2("move_backwards");
	jump.LoadAnimations2("jump");
	jump_forward.LoadAnimations2("jump_forward");
	crouch.LoadAnimations2("crouch");
	block.LoadAnimations2("blocking");
	block_crouch.LoadAnimations2("blocking_crouch");
	short_punch.LoadAnimations2("l_punch");
	m_h_punch.LoadAnimations2("m_h_punch");
	forward_m_punch.LoadAnimations2("forward_m_punch");
	forward_l_punch.LoadAnimations2("forward_l_punch");
	forward_h_punch.LoadAnimations2("forward_h_punch");
	forward_m_kick.LoadAnimations2("m_kick");
	high_kick.LoadAnimations2("h_kick");
	forward_l_kick.LoadAnimations2("forward_l_kick");
	melee_m_kick.LoadAnimations2("forward_m_kick");
	forward_h_kick.LoadAnimations2("forward_h_kick");
	crouch_l_punch.LoadAnimations2("crouch_l_m_h_punch");
	crouch_m_punch.LoadAnimations2("crouch_m_punch");
	crouch_h_punch.LoadAnimations2("crouch_h_punch");
	crouch_l_kick.LoadAnimations2("crouch_l_kick");
	crouch_m_kick.LoadAnimations2("crouch_m_kick");
	spin_kick.LoadAnimations2("crouch_h_kick");
	receive_damage_idle.LoadAnimations2("damage_idle");
	receive_damage_crouch.LoadAnimations2("damage_crouch");
	death.LoadAnimations2("ko");
	win.LoadAnimations2("victory");
	time_out.LoadAnimations2("time_over");
}

j1Player2::~j1Player2() {}

// Load assets
bool j1Player2::Start() {

	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/chunli.png");

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
	position.x = 270;
	position.y = 125;

	collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + 20, 35, 85 }, COLLIDER_PLAYER2, App->entity);

	hud = new j1Hud();
	hud->Start();

	player_start = true;
	return true;
}

//Call modules before each loop iteration
bool j1Player2::PreUpdate() {

	BROFILER_CATEGORY("PlayerPreUpdate", Profiler::Color::Orange)

		return true;
}

// Call modules on each loop iteration
bool j1Player2::Update(float dt, bool do_logic) {

	BROFILER_CATEGORY("PlayerUpdate", Profiler::Color::LightSeaGreen)

	if (player_start)
	{
		// ---------------------------------------------------------------------------------------------------------------------
		// CONTROL OF THE PLAYER
		// ---------------------------------------------------------------------------------------------------------------------
		if (!receivedDmg && !dead && !App->c_win && !App->timeOut)
		{
			// Idle
			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_IDLE
				&& App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_IDLE
				&& App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_IDLE
				&& attacking == false)
				animation = &idle;

			// Direction controls	
			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_REPEAT && attacking == false && crouching == false && blocking == false) {
				position.x += horizontalSpeed * dt;
				animation = &move_forward;
			}

			if (App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_REPEAT && attacking == false && crouching == false && blocking == false) {
				position.x -= horizontalSpeed * dt;
				animation = &move_backwards;
			}

			// Jump controls
			if (App->input->GetKey(SDL_SCANCODE_RCTRL) == j1KeyState::KEY_DOWN && jumping == false) {
				jumping = true;
				verticalSpeed = initialVerticalSpeed;
				currentJumps++;
				jumps++;
			}

			if (jumping) {
				position.y += verticalSpeed * dt;
				verticalSpeed += verticalAcceleration * dt;

				// While the player is falling
				if (!attacking) animation = &jump;
			}

			// Crouch management
			if (App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_REPEAT && attacking == false && jumping == false) {
				animation = &crouch;
				crouching = true;
				collider->rect = { (int)position.x + margin.x, 160, 35, 50 };
			}
			if (App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_UP) {
				crouch.Reset();
				crouching = false;
				collider->rect = { (int)position.x + margin.x, (int)position.y, 35, 85 };

			}

			// Punch control
			if (App->input->GetKey(SDL_SCANCODE_O) == j1KeyState::KEY_DOWN
				&& attacking == false && kicking == false && jumping == false) {
				attacking = true;
				punching = true;
				App->audio->PlayFx(attackSound);

				if (crouching) {
					animation = &crouch_l_punch;
					attackCollider = App->collisions->AddCollider({ (int)position.x - 24, (int)position.y + 43, 26, 15 }, COLLIDER_ATTACK2, App->entity); //DONE
				}
				else {
					if (App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_REPEAT) {
						animation = &forward_l_punch;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 10, (int)position.y + 23, 20, 20 }, COLLIDER_ATTACK2, App->entity); //DONE
					}
					else if (App->input->GetKey(SDL_SCANCODE_UP) == j1KeyState::KEY_REPEAT) {
						animation = &forward_h_punch;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 10, (int)position.y + 30, 20, 20 }, COLLIDER_ATTACK2, App->entity); //DONE
					}
					else {
						animation = &short_punch;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 10, (int)position.y, 25, 30 }, COLLIDER_ATTACK2, App->entity); //DONE
					}
				}
			}

			// Kick control
			if (App->input->GetKey(SDL_SCANCODE_P) == j1KeyState::KEY_DOWN
				&& attacking == false && punching == false && jumping == false) {
				attacking = true;
				kicking = true;
				App->audio->PlayFx(attackSound);

				if (crouching) {
					if (App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_REPEAT) {
						animation = &crouch_m_kick;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 27, (int)position.y + 65, 30, 25 }, COLLIDER_ATTACK2, App->entity); //DONE
					}
					else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_REPEAT) {
						animation = &spin_kick;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 25, (int)position.y + 30, 35, 22 }, COLLIDER_ATTACK2, App->entity); //DONE
					}
					else {
						animation = &crouch_l_kick;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 33, (int)position.y + 75, 35, 15 }, COLLIDER_ATTACK2, App->entity); //DONE
					}
				}
				else {
					if (App->input->GetKey(SDL_SCANCODE_UP) == j1KeyState::KEY_REPEAT) {
						animation = &high_kick;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 5, (int)position.y - margin.y, 25, 40 }, COLLIDER_ATTACK2, App->entity);
					}
					else {
						animation = &forward_m_kick;
						attackCollider = App->collisions->AddCollider({ (int)position.x - 5, (int)position.y - margin.y, 25, 40 }, COLLIDER_ATTACK2, App->entity); //DONE little rough
					}
				}

			}
		}
		// Damage management
		else if (App->timeOut && !App->r_win && !App->c_win) {
		attacking = false;
		animation = &time_out;
		position.y = 125;
		}
		else if (App->c_win) {
			attacking = false;
			animation = &win;
		}
		else if (receivedDmg) {

			if (receive_damage_idle.Finished() || receive_damage_crouch.Finished()) {
				receivedDmg = false;
				receive_damage_idle.Reset();
				receive_damage_crouch.Reset();

				if (dead) animation = &death;
			}
			else {
				position.x += horizontalSpeed * dt;
				App->audio->PlayFx(attackSound);
				if (crouching) animation = &receive_damage_crouch;
				else animation = &receive_damage_idle;
			}
		}

	// Block management
	currentTime = SDL_GetTicks();

	if (currentTime > lastTime + 500 && blocking) {
		lastTime = currentTime;
		blocking = false;
	}
	else if (blocking) {
		if (crouching) animation = &block_crouch;
		else animation = &block;
	}

		// Update collider position to player position
	if (collider != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_REPEAT && jumping == false)
		{
			collider->SetPos(position.x + margin.x, 160);
		}
		else
			collider->SetPos(position.x + margin.x, position.y + margin.y);

	}

		// ---------------------------------------------------------------------------------------------------------------------
		// DRAWING EVERYTHING ON THE SCREEN
		// ---------------------------------------------------------------------------------------------------------------------			

		// Blitting the player
		SDL_Rect r = animation->GetCurrentFrame(dt);

		if (!attacking) {
			if (crouching) {
				if (animation == &block_crouch)
					Draw(r, true, 0, 30);
				else
					Draw(r, true, 0, 22);
			}
			else if (animation == &death)
				Draw(r, true, 0, 60);
			else if (animation == &block)
				Draw(r, true, 0, 14);
			else if (animation == &time_out)
				Draw(r, false, 0, 0);
			else
				Draw(r, true, 0, 12);
		}
		else {
		if (animation == &m_h_punch)
			Draw(r, true, 0, -12);
		else if (animation == &short_punch)
			Draw(r, true, 0, -5);
		else if (animation == &forward_h_punch)
			Draw(r, true, 0, 12);
		else if (animation == &forward_l_punch)
			Draw(r, true, 0, 12);
		else if (animation == &crouch_l_punch)
			Draw(r, true, -10, 27);
		else if (animation == &forward_m_kick)
			Draw(r, true, 0, 7);
		else if (animation == &forward_l_kick)
			Draw(r, true, 0, -18);
		else if (animation == &crouch_m_kick)
			Draw(r, true, -12, 33);
		else if (animation == &crouch_l_kick)
			Draw(r, true, -12, 37);
		else if (animation == &spin_kick)
			Draw(r, true, -12, 30);
		else if (animation == &high_kick)
			Draw(r, true, 0, 5);
		}

		// Punch management
		if (crouch_l_punch.Finished() || m_h_punch.Finished() || short_punch.Finished()
			|| forward_h_punch.Finished() || forward_l_punch.Finished()) {

			crouch_m_punch.Reset();
			m_h_punch.Reset();
			short_punch.Reset();
			crouch_l_punch.Reset();
			forward_l_punch.Reset();
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

			if (attackCollider != nullptr)
				attackCollider->to_delete = true;
		}

		hud->Update(dt);

		// We update the camera to followe the player every frame
		UpdateCameraPosition();


		if (block.Finished() && blocked_idle && App->entity->player->attackCollider->to_delete)
			blocked_idle = false;
		if (block_crouch.Finished() && blocked_crouch && App->entity->player->attackCollider->to_delete)
			blocked_crouch = false;

		return true;
	}
}

// Call modules after each loop iteration
bool j1Player2::PostUpdate() {

	BROFILER_CATEGORY("PlayerPostUpdate", Profiler::Color::Yellow)

		return true;
}

// Load game state
bool j1Player2::Load(pugi::xml_node& data) {

	return true;
}

// Save game state
bool j1Player2::Save(pugi::xml_node& data) const {

	pugi::xml_node j = data.append_child("jumps");
	j.append_attribute("jumps") = jumps;

	pugi::xml_node b = data.append_child("blocks");
	b.append_attribute("blocks") = blocks;

	pugi::xml_node d = data.append_child("damage_taken");
	d.append_attribute("damage_taken") = damage_taken;

	return true;
}

// Called before quitting
bool j1Player2::CleanUp() {

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

void j1Player2::UpdateCameraPosition()
{
	if (position.y > 125) {
		position.y = 125;
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
void j1Player2::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_1->type == COLLIDER_PLAYER2 || col_1->type == COLLIDER_ATTACK2)
	{
		if (col_2->type == COLLIDER_ATTACK1) {
			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_REPEAT) {

				if (crouching)
				{
					animation = &block_crouch;
					blocking = true;
					if (!blocked_crouch)
					{
						blocks++;
						blocked_crouch = true;
					}
				}
				else
				{
					animation = &block;
					blocking = true;
					if (!blocked_idle)
					{
						blocks++;
						blocked_idle = true;
					}
				}
			}
			else if (!blocking && !receivedDmg) {
				receivedDmg = true;
				attacking = false;

				C_PointsToSubstract += 60;
				life -= 60;

				if (life < 0) life = 0;

				if (life == 0) {
					dead = true;
					App->r_win = true;
				}
				else {
					if (crouching) animation = &receive_damage_crouch;
					else animation = &receive_damage_idle;
				}

				damage_taken++;
			}
		}
	}
};

void j1Player2::LoadPlayerProperties() {

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