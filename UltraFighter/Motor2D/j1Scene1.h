#ifndef __j1SCENE1_H__
#define __j1SCENE1_H__

#include "j1Module.h"
#include "j1Button.h"
#include "j1Timer.h"
#include "PugiXml/src/pugixml.hpp"

struct SDL_Texture;
struct _TTF_Font;
class j1Box;

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

	// Called to change scene
	void ChangeSceneMenu();

	// Load and Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	j1Box* settings_window = nullptr;
	fPoint initialScene1Position;
	bool player_created = false;
	bool backToMenu = false;

	p2List<j1Button*> scene1Buttons;
	p2List<j1Label*> scene1Labels;
	p2List<j1Box*> scene1Boxes;

	j1Timer	startup_time;
	int time_scene1;
	bool changingScene = false;

private:
	SDL_Texture * debug_tex = nullptr;
	SDL_Texture* gui_tex = nullptr;
	

	_TTF_Font* font = nullptr;

	bool closeSettings = false;
	bool continueGame = true;
	bool resettingLevel = false;	

public:

	SDL_Texture * graphics = nullptr;
	SDL_Rect background;
	SDL_Rect sky;
	SDL_Rect grid;
	Animation couple_left;
	Animation guys;
	Animation couple_right;
};

#endif // __j1SCENE1_H__