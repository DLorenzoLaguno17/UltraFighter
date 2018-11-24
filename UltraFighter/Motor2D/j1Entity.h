#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "j1EntityManager.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "p2Animation.h"
#include "p2Log.h"

struct Collider;
struct Animation;
struct SDL_Texture;

enum ENTITY_TYPES;

class j1Entity
{
public:
	j1Entity(int x, int y, ENTITY_TYPES type);

	virtual bool Start() {
		return true;
	};
	virtual bool Update(float dt, bool do_logic) {
		return true;
	};
	virtual bool PostUpdate() {
		return true;
	};
	virtual bool PreUpdate() {
		return true;
	};
	virtual bool CleanUp() {
		return true;
	};

public:

	virtual bool Load(pugi::xml_node&) { return true; };
	virtual bool Save(pugi::xml_node&) const { return true; };

	virtual void Draw(SDL_Rect r, bool flip = false, int x = 0, int y = 0);
	virtual void OnCollision(Collider* c1, Collider* c2) {};

	ENTITY_TYPES type;

	fPoint position;
	fPoint initialPosition;
	float speed;

	Animation* animation = nullptr;
	Collider* collider = nullptr;
	SDL_Texture* sprites = nullptr;

};

#endif // __ENTITY_H__