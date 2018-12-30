#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "j1App.h"

struct SDL_Texture;

class j1Map :public j1Module {

public:

	j1Map();
	virtual ~j1Map();

	bool Start();
	void Draw();

public:

	SDL_Texture * graphics = nullptr;
	SDL_Rect background;
	SDL_Rect sky;
	SDL_Rect box;
	SDL_Rect grid;
	Animation couple_left;
	Animation guys;
	Animation couple_right;

};

#endif // __j1MAP_H__