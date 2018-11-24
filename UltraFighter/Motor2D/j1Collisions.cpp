#include "j1Collisions.h"
#include "p2Defs.h"
#include "j1Module.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Map.h"

#include "Brofiler/Brofiler.h"
#include "SDL\include\SDL.h"

j1Collisions::j1Collisions() : j1Module()
{

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		colliders[i] = nullptr;
	}

	name.create("collisions");

	matrix[COLLIDER_NONE][COLLIDER_NONE] = false;
	matrix[COLLIDER_NONE][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_NONE][COLLIDER_WALL] = false;
	matrix[COLLIDER_NONE][COLLIDER_DEATH] = false;
	matrix[COLLIDER_NONE][COLLIDER_WIN] = true;
	matrix[COLLIDER_NONE][COLLIDER_HOOK] = false;
	matrix[COLLIDER_NONE][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_NONE][COLLIDER_ATTACK] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_NONE] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_WIN] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_HOOK] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ATTACK] = true;
	
	matrix[COLLIDER_DEATH][COLLIDER_NONE] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DEATH][COLLIDER_WALL] = false;
	matrix[COLLIDER_DEATH][COLLIDER_DEATH] = false;
	matrix[COLLIDER_DEATH][COLLIDER_WIN] = false;
	matrix[COLLIDER_DEATH][COLLIDER_HOOK] = false;
	matrix[COLLIDER_DEATH][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_DEATH][COLLIDER_ATTACK] = false;

	matrix[COLLIDER_WIN][COLLIDER_NONE] = true;
	matrix[COLLIDER_WIN][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WIN][COLLIDER_WALL] = false;
	matrix[COLLIDER_WIN][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WIN][COLLIDER_WIN] = false;
	matrix[COLLIDER_WIN][COLLIDER_HOOK] = false;
	matrix[COLLIDER_WIN][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_WIN][COLLIDER_ATTACK] = false;

	matrix[COLLIDER_WALL][COLLIDER_NONE] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WALL][COLLIDER_WIN] = false;
	matrix[COLLIDER_WALL][COLLIDER_HOOK] = true;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_WALL][COLLIDER_ATTACK] = false;

	matrix[COLLIDER_HOOK][COLLIDER_HOOK] = false; 
	matrix[COLLIDER_HOOK][COLLIDER_NONE] = false;
	matrix[COLLIDER_HOOK][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_HOOK][COLLIDER_WIN] = false;
	matrix[COLLIDER_HOOK][COLLIDER_DEATH] = false;
	matrix[COLLIDER_HOOK][COLLIDER_WALL] = true;
	matrix[COLLIDER_HOOK][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_HOOK][COLLIDER_ATTACK] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_NONE] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_DEATH] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_WIN] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_HOOK] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ATTACK] = true;

	matrix[COLLIDER_ATTACK][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_NONE] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_WALL] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_DEATH] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_WIN] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_ATTACK][COLLIDER_HOOK] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_PLAYER] = false;
}

j1Collisions::~j1Collisions() {}

bool j1Collisions::PreUpdate()
{
	BROFILER_CATEGORY("CollisionPreUpdate", Profiler::Color::Orange)

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr) 
		{
			if (colliders[i]->to_delete) 
			{
				delete colliders[i];
				colliders[i] = nullptr;
			}
		}
	}

	Collider* collider1;
	Collider* collider2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr) continue;

		if (colliders[i]->type == COLLIDER_PLAYER || colliders[i]->type == COLLIDER_NONE || colliders[i]->type == COLLIDER_HOOK || colliders[i]->type == COLLIDER_ENEMY)
		{
			collider1 = colliders[i];

			for (uint j = 0; j < MAX_COLLIDERS; ++j)
			{
				if (colliders[j] == nullptr || i == j) continue;

				collider2 = colliders[j];

				if (collider1->CheckCollision(collider2->rect) == true)
				{
					if (matrix[collider1->type][collider2->type] && collider1->callback)
					{
						collider1->callback->OnCollision(collider1, collider2);
					}
				}
			}
		}
	}

	return true;
}

bool j1Collisions::Update(float dt) 
{
	BROFILER_CATEGORY("CollisionUpdate", Profiler::Color::LightSeaGreen)

	DrawColliders();

	return true;
}

bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

void j1Collisions::DrawColliders() 
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)	
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE:		//White
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_WALL:		//Blue															
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_WIN:		//Green															
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_DEATH:	//Red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_PLAYER:	//Yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		case COLLIDER_HOOK:		//Black
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
			break;
		case COLLIDER_ENEMY:	//Orange
			App->render->DrawQuad(colliders[i]->rect, 253, 106, 2, alpha);
			break;
		case COLLIDER_ATTACK:	//Cyan
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
			break;
		}
	}
}

Collider* j1Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{

		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}
	return ret;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

COLLISION_DIRECTION Collider::CheckDirection(const SDL_Rect& r) const 
{
	//r wall
	//rect enemic

	if (r.x + r.w < rect.x)
	{
		return LEFT_COLLISION;
	}

	else if (r.x > rect.x + rect.w)
	{
		return RIGHT_COLLISION;
	}

	else if (r.y <= rect.y + rect.h && rect.x > r.x)
	{
		return UP_COLLISION;
	}

	

	else return NONE_COLLISION;
}