#ifndef __j1SCENEMENU_H__
#define __j1SCENEMENU_H__

#include "j1Module.h"
#include "j1Render.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2Animation.h"

struct SDL_Texture;
struct _TTF_Font;
class j1Box;
class j1Button;
class j1Label;

enum SCENE {
	MENU = 0,
	SCENE1,
};

class j1SceneMenu : public j1Module
{
public:
	j1SceneMenu();

	virtual ~j1SceneMenu();

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
	void ChangeScene(SCENE objectiveScene);

	// Load and Save
	bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

public:
	j1Box* settings_window = nullptr;

	p2List<j1Button*> menuButtons; 
	p2List<j1Label*> menuLabels;
	p2List<j1Box*> menuBoxes;

private:

	SDL_Texture* gui_tex = nullptr;
	SDL_Texture* player_tex = nullptr;
	SDL_Texture* harpy_tex = nullptr;
	SDL_Texture* background = nullptr;

	_TTF_Font* font = nullptr;

	bool continueGame = true;
	bool startGame = false;
	bool loadGame = false;
	bool openCredits = false;
	bool player_created = false;

	Animation player;
	Animation harpy;

	SCENE currentScene = SCENE::MENU;
};

#endif //__j1SCENEMENU_H__