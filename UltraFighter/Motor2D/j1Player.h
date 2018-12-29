#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;
class j1Hud;

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
	Animation move_forward;
	Animation move_backwards;
	Animation jump;
	Animation jump_forward;
	Animation crouch;
	Animation blocking;
	Animation l_punch;
	Animation m_h_punch;
	Animation forward_m_punch;
	Animation forward_l_punch;
	Animation forward_h_punch;
	Animation l_m_kick;
	Animation h_kick;
	Animation forward_l_kick;
	Animation forward_m_kick;
	Animation forward_h_kick;
	Animation crouch_l_punch;
	Animation crouch_m_punch;
	Animation crouch_h_punch;
	Animation crouch_l_kick;
	Animation crouch_m_kick;
	Animation crouch_h_kick;

	// Sounds
	uint deathSound;
	uint jumpSound;
	uint playerHurt;
	uint attackSound;
	uint lifeup;

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
	uint points = 0;
	uint score_points = 0;
	uint lives;

	Collider* attackCollider = nullptr;

	j1Hud* hud = nullptr;

	// Attack values
	int attackBlittingX;
	int attackBlittingY;
	int rightAttackSpawnPos;
	int leftAttackSpawnPos;

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
	bool playerIdle = false;
	bool jumping = false;
	bool crouching = false;

	bool player_start = false;
	bool dead = false;
	bool playedSound = false;
	bool attacking = false;
	bool punching = false;
	bool kicking = false;

	int cameraLimit;

private:
	int playerLimit;

	bool loadedAudios = false;
};

#endif // __jPLAYER_H__