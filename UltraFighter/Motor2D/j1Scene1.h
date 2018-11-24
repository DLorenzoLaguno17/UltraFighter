#ifndef __j1SCENE1_H__
#define __j1SCENE1_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene1 : public j1Module
{
public:

	j1Scene1();

	// Destructor
	virtual ~j1Scene1();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	fPoint initialScene1Position;
	bool player_created = false;

private:

	SDL_Texture* debug_tex;

};

#endif // __j1SCENE1_H__