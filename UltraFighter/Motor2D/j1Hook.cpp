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
#include <cassert>

#include "Brofiler/Brofiler.h"

j1Hook::j1Hook(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::HOOK)
{
	animation = NULL;

	throwHook.LoadAnimations("throwHook");
	returnHook.LoadAnimations("returnHook");
	dragHookRight.LoadAnimations("dragHookRight");
	dragHookLeft.LoadAnimations("dragHookLeft");
}

j1Hook::~j1Hook() {}

// Load assets
bool j1Hook::Start() {
	
	LoadHookProperties();

	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/hook.png");

	// Audios are loaded
	if (!loadedAudios) {
		chain = App->audio->LoadFx("audio/fx/chain.wav");
		loadedAudios = true;
	}

	animation = &throwHook;
	
	collider = App->collisions->AddCollider({NULL, NULL, hookSize.x, hookSize.y}, COLLIDER_HOOK, App->entity);

	return true;
}

// Call modules on each loop iteration
bool j1Hook::Update(float dt, bool do_logic) {	

	BROFILER_CATEGORY("HookUpdate", Profiler::Color::LightSeaGreen)

	// We reset the values if the player has arrived to it's hooked destination
	if (somethingHit) {
		if (arrived || (animation == &dragHookLeft && dragHookLeft.Finished()) 
			|| (animation == &dragHookRight && dragHookRight.Finished())) {
			somethingHit = false;
			thrown = false;
			throwHook.Reset();
			dragHookRight.Reset();
			dragHookLeft.Reset();
			colliderPosition = initialColliderPosition;
		}
	}

	// Check if the key is pressed and the hook can be thrown
	if ((App->input->GetKey(SDL_SCANCODE_O) == j1KeyState::KEY_DOWN
		|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_B)) == KEY_DOWN)
		&& thrown == false && App->entity->player->GodMode == false 
		&& App->entity->player->dead == false) {

		colliderPosition = initialColliderPosition;
		thrown = true;
		animation = &throwHook;
		App->audio->PlayFx(chain);
	}

	// If the hook is thrown
	if (thrown) {
		collider->type = COLLIDER_HOOK;

		// If the hook hits something
		if (somethingHit) {			
			if (!arrived) {
				if (App->entity->player->facingRight) {
					if (App->entity->player->position.x < objectivePosition) {
						App->entity->player->position.x += hookSpeed * dt;
						animation = &dragHookRight;
					}
				}
				else if (App->entity->player->position.x > objectivePosition) {
					App->entity->player->position.x -= hookSpeed * dt;
					animation = &dragHookLeft;
				}
			}
		}
		// If the hook doesn't hit anything
		else {

			if (throwHook.Finished()) {
				animation = &returnHook;
				throwHook.Reset();
			}

			if (returnHook.Finished()) {
				thrown = false;
				returnHook.Reset();
			}
		}

		SDL_Rect r = animation->GetCurrentFrame(dt);

		assert(animation->speed > 0.002, "Speed too slow");

		// Blitting the hook if it is still thrown
		if (thrown) {
			if (App->entity->player->facingRight) {
				position.x = App->entity->player->position.x + spawnPositionRight.x;
				position.y = App->entity->player->position.y + spawnPositionRight.y;

				Draw(r);

				// Update collider position to hook's claw position with the player facing right
				if (!somethingHit) {
					if (animation == &throwHook)
						colliderPosition += hookSpeed * dt;
					else if (animation == &returnHook)
						colliderPosition -= hookSpeed * dt;

					collider->SetPos(position.x + colliderPosition, position.y + heightMargin);
				}
			}
			else {
				position.x = App->entity->player->position.x + spawnPositionLeft.x;
				position.y = App->entity->player->position.y + spawnPositionLeft.y;

				if (animation == &dragHookLeft)
					Draw(r);
				else
					Draw(r, true);

				// Update collider position to hook's claw position with the player facing left
				if (!somethingHit) {
					if (animation == &throwHook)
						colliderPosition -= hookSpeed * dt;
					else if (animation == &returnHook)
						colliderPosition += hookSpeed * dt;

					collider->SetPos(position.x + leftMargin + colliderPosition, position.y + heightMargin);
				}
			}
		}
	}
	// If the hook is not thrown
	else {
		if (App->entity->player->facingRight) {
			position.x = App->entity->player->position.x + spawnPositionRight.x;
			position.y = App->entity->player->position.y + spawnPositionRight.y;

			collider->SetPos(position.x, position.y + heightMargin);
		}
		else {
			position.x = App->entity->player->position.x + spawnPositionLeft.x;
			position.y = App->entity->player->position.y + spawnPositionLeft.y;

			collider->SetPos(position.x + leftMargin, position.y + heightMargin);
		}
			
		collider->type = COLLIDER_NONE;
	}
	
	return true;
}

// Called before quitting
bool j1Hook::CleanUp() {

	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(sprites);
	if (collider != nullptr)
		collider->to_delete = true;

	if (animation != nullptr)
		animation = nullptr;

	return true;
}

// Detects collisions
void j1Hook::OnCollision(Collider* col_1, Collider* col_2) {
	if (col_1->type == COLLIDER_HOOK && animation != &returnHook) {
		
		// First we check if the collision is perpendicular
		if (col_1->rect.y + col_1->rect.h >= col_2->rect.y
			&& col_1->rect.y <= col_2->rect.y + col_2->rect.h){

				// If the hook hits right
			if ((App->entity->player->facingRight == true
				&& col_1->rect.x + col_1->rect.w >= col_2->rect.x
				&& col_1->rect.x <= col_2->rect.x) ||
				// If the hook hits left
				(App->entity->player->facingRight == false &&
				col_1->rect.x <= col_2->rect.x + col_2->rect.w
				&& col_1->rect.x + col_1->rect.w >= col_2->rect.x + col_2->rect.w)) {

				somethingHit = true;
				arrived = false;
				App->entity->player->feetOnGround = true;
				App->entity->player->fallingSpeed = App->entity->player->initialFallingSpeed;
				App->entity->player->currentJumps = App->entity->player->initialJumps;

				if (App->entity->player->facingRight)
					objectivePosition = col_2->rect.x - App->entity->player->collider->rect.w - App->entity->player->colisionMargin;
				else
					objectivePosition = col_2->rect.x + col_2->rect.w - App->entity->player->colisionMargin;
			}
		}
	}
}

void j1Hook::LoadHookProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node hook;
	hook = config.child("hook");

	// Copying variables
	hookSpeed = hook.attribute("speed").as_int();
	leftMargin = hook.attribute("leftMargin").as_int();

	// Copying collider values
	hookSize.x = hook.child("size").attribute("width").as_int();
	hookSize.y = hook.child("size").attribute("height").as_int();
	initialColliderPosition = hook.child("collider").attribute("colliderPosition").as_int();
	heightMargin = hook.child("collider").attribute("heightMargin").as_int();

	// Copying the spawning positions
	spawnPositionRight.x = hook.child("spawnPositionRight").attribute("x").as_int();
	spawnPositionRight.y = hook.child("spawnPositionRight").attribute("y").as_int();
	spawnPositionLeft.x = hook.child("spawnPositionLeft").attribute("x").as_int();
	spawnPositionLeft.y = hook.child("spawnPositionLeft").attribute("y").as_int();
}
