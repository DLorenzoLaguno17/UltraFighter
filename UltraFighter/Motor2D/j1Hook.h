#ifndef __j1HOOK_H__
#define __j1HOOK_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

class j1Hook : public j1Entity
{

public:

	j1Hook(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1Hook();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt, bool do_logic);

	// Called before quitting
	bool CleanUp();

	// Called to check collisions
	void OnCollision(Collider* col_1, Collider* col_2);

	void LoadHookProperties();

public:

	// Hook animations
	Animation throwHook;
	Animation returnHook;
	Animation dragHookRight;
	Animation dragHookLeft;

	int objectivePosition;
	int hookSpeed;
	int initialColliderPosition;
	int colliderPosition;
	int leftMargin;
	int heightMargin;
	
	iPoint spawnPositionRight;
	iPoint spawnPositionLeft;
	iPoint hookSize;

	bool somethingHit = false;
	bool thrown = false;
	bool arrived = false;

	// Audios
	uint chain;
	bool loadedAudios;
};

#endif // __j1HOOK_H__
