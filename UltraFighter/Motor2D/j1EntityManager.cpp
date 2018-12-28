#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "j1Scene1.h"
#include "j1Player.h"
#include "j1Hook.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager()
{
	name.create("entityManager");
}

j1EntityManager::~j1EntityManager() {}

bool j1EntityManager::Start()
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{ 
		iterator->data->Start();
	}

	return true;
}

bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManagerPreUpdate", Profiler::Color::Orange)

	return true;
}

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Awaking Entity manager");
	updateMsCycle = config.attribute("updateMsCycle").as_float();

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManagerUpdate", Profiler::Color::LightSeaGreen)

	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle)
		do_logic = true;

	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->Update(dt, do_logic);
	}

	if (do_logic) {
		accumulatedTime = 0.0f;
		do_logic = false;
	}

	return true;
}

bool j1EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntityManagerPostUpdate", Profiler::Color::Yellow)

	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	LOG("Freeing all enemies");

	
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next) 
	{
		iterator->data->CleanUp();
	}

	entities.clear();

	player = nullptr;
	hook = nullptr;

	return true;
}

j1Entity* j1EntityManager::CreateEntity(ENTITY_TYPES type, int x, int y)
{
	j1Entity* ret = nullptr;
	switch (type)
	{
	case PLAYER: 
		ret = new j1Player(x, y, type);
	if (ret != nullptr) entities.add(ret); break;

	}
	return ret;
}

void j1EntityManager::AddEnemy(int x, int y, ENTITY_TYPES type)
{
	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type == ENTITY_TYPES::UNKNOWN)
		{
			queue[i].type = type;
			queue[i].position.x = x;
			queue[i].position.y = y;
			break;
		}
	}
}

void j1EntityManager::CreatePlayer() 
{
	player = (j1Player*)CreateEntity(PLAYER);
}

void j1EntityManager::OnCollision(Collider* c1, Collider* c2)
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		if (iterator->data->collider == c1) 
		{ 
			iterator->data->OnCollision(c1, c2); 	
			break; 
		}
	}
}

bool j1EntityManager::Load(pugi::xml_node& data)
{
	if (player != nullptr)
	{
		player->Load(data);
	}

	return true;
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	player->Save(data.append_child("player"));

	return true;
}