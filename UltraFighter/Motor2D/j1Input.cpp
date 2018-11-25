#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Window.h"
#include "SDL/include/SDL.h"

#include "Brofiler/Brofiler.h"

#define MAX_KEYS 300

j1Input::j1Input() : j1Module()
{
	keyboard = new j1KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(j1KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(j1KeyState) * NUM_MOUSE_BUTTONS);

	name.create("input");
}

// Destructor
j1Input::~j1Input()
{
	delete[] keyboard;
}

// Called before render is available
bool j1Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);
	SDL_Init(SDL_INIT_GAMECONTROLLER);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
		LOG("SDL_INIT_JOYSTICK could not initialize , SDL Error: %s\n", SDL_GetError());
	}

	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(0))
		{
			controller = SDL_GameControllerOpen(0);
		}
		if (SDL_IsGameController(1)) {
			controller2 = SDL_GameControllerOpen(1);
			break;
		}

	}

	if (controller == NULL) {
		LOG(" Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		use_controller = false;
	}

	return ret;
}

// Called before the first frame
bool j1Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool j1Input::PreUpdate()
{
	BROFILER_CATEGORY("InputPreUpdate", Profiler::Color::Orange)

	SDL_PumpEvents();

	static SDL_Event event;

	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	//CONTROLLER INPUT
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			if (i == 0)
			{
				if (SDL_IsGameController(i))
				{
					controller = SDL_GameControllerOpen(i);
					if (SDL_GameControllerGetAttached(controller))
					{
						gamepadP1LAxisX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
						gamepadP1LAxisY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
						gamepadP1APressed = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
						gamepadP1BPressed = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
						gamepadP1YPressed = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);
						gamepadP1StartPressed = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);
						gamepadP1con = true;
					}
					else
					{
						SDL_GameControllerClose(controller);
						controller = nullptr;
						gamepadP1con = false;
					}
				}
			}
			else if (i<1)
			{
				gamepadP2con = false;
				SDL_GameControllerClose(controller2);
				controller2 = nullptr;
				gamepadP2con = false;
			}
			else if (i == 1 || i == 0 && gamepadP1con == false)
			{
				if (SDL_IsGameController(i))
				{
					controller2 = SDL_GameControllerOpen(i);
					if (SDL_GameControllerGetAttached(controller2))
					{
						gamepadP2LAxisX = SDL_GameControllerGetAxis(controller2, SDL_CONTROLLER_AXIS_LEFTX);
						gamepadP2LAxisY = SDL_GameControllerGetAxis(controller2, SDL_CONTROLLER_AXIS_LEFTY);
						gamepadP2APressed = SDL_GameControllerGetButton(controller2, SDL_CONTROLLER_BUTTON_A);
						gamepadP2BPressed = SDL_GameControllerGetButton(controller2, SDL_CONTROLLER_BUTTON_B);
						gamepadP2YPressed = SDL_GameControllerGetButton(controller2, SDL_CONTROLLER_BUTTON_Y);
						gamepadP2StartPressed = SDL_GameControllerGetButton(controller2, SDL_CONTROLLER_BUTTON_START);
						gamepadP2con = true;
						break;
					}
					else
					{
						SDL_GameControllerClose(controller2);
						controller2 = nullptr;
						gamepadP2con = false;
					}
				}
			}
		}
	}


	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEMOTION:
				int scale = App->win->GetScale();
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = event.motion.x / scale;
				mouse_y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;
		}
	}

	return true;
}

// Called before quitting
bool j1Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_GameControllerClose(controller);
	controller = nullptr;
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool j1Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

void j1Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void j1Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}