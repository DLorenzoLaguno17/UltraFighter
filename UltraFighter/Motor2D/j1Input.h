#ifndef __j1INPUT_H__
#define __j1INPUT_H__

#include "j1Module.h"
#include "SDL\include\SDL_scancode.h"
#include "SDL\include\SDL.h"

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
#define MAX_BUTTONS 100
#define DEAD_ZONE 0.2f

struct SDL_Rect;

enum j1EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum j1KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum j1GamepadButtons {
	ARROW_UP = 0,
	ARROW_DOWN,
	ARROW_RIGHT,
	ARROW_LEFT,
	BUTTON_A,
	BUTTON_B,
	BUTTON_START
};

class j1Input : public j1Module
{

public:

	j1Input();

	// Destructor
	virtual ~j1Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(j1EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	j1KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	j1KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

public:
	bool		windowEvents[WE_COUNT];
	j1KeyState*	keyboard;
	j1KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x;
	int			mouse_motion_y;
	int			mouse_x;
	int			mouse_y;

	//p1 controller
	SDL_GameController* controller = nullptr;
	bool gamepadP1con = false;
	int gamepadP1LAxisX = 0;
	int gamepadP1LAxisY = 0;
	bool gamepadP1APressed = false;
	bool gamepadP1BPressed = false;
	bool gamepadP1YPressed = false;
	bool gamepadP1StartPressed = false;
	//p2 controller
	SDL_GameController* controller2 = nullptr;
	bool gamepadP2con = false;
	int gamepadP2LAxisX = 0;
	int gamepadP2LAxisY = 0;
	bool gamepadP2APressed = false;
	bool gamepadP2BPressed = false;
	bool gamepadP2YPressed = false;
	bool gamepadP2StartPressed = false;

	//SDL_Joystick *joystick = SDL_JoystickOpen(0);

	bool use_controller = false;
	bool use_controller2 = false;
	int controller_index = 0;
};

#endif // __j1INPUT_H__