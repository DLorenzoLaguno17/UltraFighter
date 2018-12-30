#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "j1Module.h"
#include "j1UserInterfaceElement.h"

#define MAX_ENTITIES 1000

class j1Entity;
class j1Player;
class j1Player2;
class j1Hook;

struct SDL_Texture;

enum ENTITY_TYPES
{
	PLAYER1,
	PLAYER2,
	UNKNOWN
};

struct EntityInfo
{
	ENTITY_TYPES type = ENTITY_TYPES::UNKNOWN;
	iPoint position;
};

class j1EntityManager : public j1Module
{
public:
	// Constructor
	j1EntityManager();

	// Destructor
	~j1EntityManager();

	bool Awake(pugi::xml_node&);
	bool Start();

	// Called every frame
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	j1Entity* CreateEntity(ENTITY_TYPES type, int x = 0, int y = 0);

	void OnCollision(Collider* c1, Collider* c2);
	void CreatePlayer();
	void AddEnemy(int x, int y, ENTITY_TYPES type);

public:

	p2List<j1Entity*>	entities;
	j1Player*			player = nullptr;
	j1Player2*			player2 = nullptr;

	SDL_Texture*		player_texture = nullptr;
	SDL_Texture*		harpy_texture = nullptr;

private:

	EntityInfo			queue[MAX_ENTITIES];
	bool				do_logic = false;
	float				accumulatedTime = 0.0f;
	float				updateMsCycle = 0.0f;

};

#endif // __J1ENTITYMANAGER_H__