#ifndef __j1COLLISIONS_H__
#define __j1COLLISIONS_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL\include\SDL_rect.h"

#define MAX_COLLIDERS 600

enum COLLIDER_TYPE
{
	COLLIDER_NONE,
	COLLIDER_WALL,
	COLLIDER_PLAYER1,
	COLLIDER_PLAYER2,
	COLLIDER_ATTACK1,
	COLLIDER_ATTACK2,
	COLLIDER_BLOCK1,
	COLLIDER_BLOCK2,
	COLLIDER_MAX,
};

enum ATTACK_TYPE {
	NO_ATTACK,
	PUNCH,
	KICK,
	SPECIAL,
	BLOCK
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;

	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) : rect(rectangle), type(type), callback(callback) {}

	void SetPos(int x, int y) { rect.x = x; rect.y = y; }
	bool CheckCollision(const SDL_Rect& r) const;
};

struct Attack {
	ATTACK_TYPE type;
	uint damage;
	uint priority;
	Collider* collider = nullptr;

	Attack(ATTACK_TYPE type, uint damage, uint priority, Collider* collider = nullptr) : 
		type(type), damage(damage), priority(priority), collider(collider) {}

	// Checks the attack priority among itself and another attack
	Attack* priorityAttack(Attack* interactingAttack) {
		if (interactingAttack->priority > priority)
			return interactingAttack;
		else
			return this;
	}
};

class j1Collisions : public j1Module
{
public:

	j1Collisions();
	~j1Collisions();

	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void DrawColliders();
	bool debug = false;


private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
};


#endif // __j1COLLISIONS_H__