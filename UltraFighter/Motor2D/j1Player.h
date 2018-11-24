#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

class j1Player : public j1Entity
{

public:

	j1Player(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1Player();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Called to check collisions
	void OnCollision(Collider* col_1, Collider* col_2);

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void LoadPlayerProperties();
	void UpdateCameraPosition();

public:

	// Animations of the player
	Animation idle;
	Animation run;
	Animation jump;
	Animation fall;
	Animation godmode;
	Animation attackRight;
	Animation attackLeft;
	Animation death;

	// Sounds
	uint deathSound;
	uint jumpSound;
	uint playerHurt;
	uint attackSound;

	// To know the last direction the character was moving to
	bool facingRight = true;

	fPoint initialPosition;

	// Size of the player collider, where x = w and y = h
	iPoint playerSize;
	iPoint margin;

	uint currentJumps;
	uint initialJumps;
	uint maxJumps;
	uint colisionMargin;
	uint deathByFallColliderHeight;

	Collider* attackCollider = nullptr;

	// Attack values
	int attackBlittingX;
	int attackBlittingY;
	int rightAttackSpawnPos;
	int leftAttackSpawnPos;

	float godModeSpeed;
	float horizontalSpeed;
	//Jumping speed
	float initialVerticalSpeed;
	float verticalSpeed;
	// Free fall speed
	float fallingSpeed;
	float initialFallingSpeed;
	// "Gravity"
	float verticalAcceleration;

	// It tells you wether the player has landed, has a wall in front, a wall behind or a wall above
	bool feetOnGround = false;
	bool wallInFront = false;
	bool wallBehind = false;
	bool wallAbove = false;

	bool GodMode = false;
	bool playerIdle = false;
	bool jumping = false;
	bool freefall = false;

	bool player_start = false;
	bool loading = false;
	bool dead = false;
	bool playedSound = false;
	bool deathByFall = false;
	bool attacking = false;

private:
	int cameraLimit;
	int playerLimit;

	bool loadedAudios = false;
};

#endif // __jPLAYER_H__