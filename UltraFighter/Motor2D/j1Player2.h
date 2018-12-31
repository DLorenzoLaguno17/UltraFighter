#ifndef __j1PLAYER2_H__
#define __j1PLAYER2_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;
class j1Hud;

class j1Player2 : public j1Entity
{

public:
	j1Player2(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1Player2();

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
	Animation block;
	Animation block_crouch;
	Animation short_punch;
	Animation m_h_punch;
	Animation forward_m_punch;
	Animation forward_l_punch;
	Animation forward_h_punch;
	Animation forward_m_kick;
	Animation high_kick;
	Animation forward_l_kick;
	Animation melee_m_kick;
	Animation forward_h_kick;
	Animation crouch_l_punch;
	Animation crouch_m_punch;
	Animation crouch_h_punch;
	Animation crouch_l_kick;
	Animation crouch_m_kick;
	Animation spin_kick;
	Animation receive_damage_idle;
	Animation receive_damage_crouch;

	// Sounds
	uint deathSound;
	uint jumpSound;
	uint playerHurt;
	uint attackSound;
	uint attackSoundmiss;
	uint lifeup;

	// To know the last direction the character was moving to
	bool facingRight = true;

	// Size of the player collider, where x = w and y = h
	iPoint playerSize;
	iPoint margin;

	uint currentTime, lastTime = 0;

	uint currentJumps;
	uint initialJumps;
	uint C_PointsToSubstract = 0;

	Collider* attackCollider = nullptr;

	j1Hud* hud = nullptr;

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
	bool blocking = false;

	bool player_start = false;
	bool dead = false;
	bool playedSound = false;
	bool attacking = false;
	bool punching = false;
	bool kicking = false;
	bool receivedDmg = false;

private:

	bool loadedAudios = false;
};

#endif // __jPLAYER2_H__