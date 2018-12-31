#ifndef __j1HUD_H__
#define __j1HUD_H__

#include "j1Timer.h"
#include "p2SString.h"
#include "p2List.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2Animation.h"

class j1Button;
class j1Label;

struct _TTF_Font;
struct SDL_Texture;

class j1Hud
{
public:
	j1Hud();
	~j1Hud();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	SDL_Texture* Ryu = nullptr;
	SDL_Texture* Chunli = nullptr;
	SDL_Texture* vs = nullptr;
	SDL_Texture* Ryulifegood = nullptr;
	SDL_Texture* Ryulifebad = nullptr;
	SDL_Texture* Chunlilifegood = nullptr;
	SDL_Texture* Chunlilifebad = nullptr;
	j1Timer	startup_time;
	
	p2SString time_text;
	p2SString min_text;
	p2SString min_text_left = "0";
	p2SString score;
	p2SString score_points;

	uint R_lifepoints;
	uint C_lifepoints;
	uint C_startlifepoints;

	j1Label* seconds;
	j1Label* minutes;

	_TTF_Font* text = nullptr;
	
	int time;
	int min = 00;

	Animation* animation = nullptr;
	Animation idle;

	p2List<j1Label*> labels_list;
};


#endif // __j1LHUD_H__