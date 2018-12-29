#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collisions.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Hook.h"
#include "j1Player.h"
#include "j1SceneMenu.h"
#include "j1Scene1.h"
#include "j1FadeToBlack.h"
#include "j1Gui.h"
#include "j1SceneMenu.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Button.h"
#include "j1Box.h"

#include "Brofiler/Brofiler.h"

j1Scene1::j1Scene1() : j1Module()
{
	name.create("scene1");
}

// Destructor
j1Scene1::~j1Scene1()
{}

// Called before render is available
bool j1Scene1::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene 1");
	bool ret = true;

	if (App->menu->active == true)
		active = false;

	if (active == false)
		LOG("Scene1 not active.");

	// Copying the position of the player
	initialScene1Position.x = config.child("initialPlayerPosition").attribute("x").as_int();
	initialScene1Position.y = config.child("initialPlayerPosition").attribute("y").as_int();

	return ret;
}

// Called before the first frame
bool j1Scene1::Start()
{
	if (active)
	{
		// The map is loaded
		App->map->Load("lvl1.tmx");

		// The audio is played	
		App->audio->PlayMusic("audio/music/level1_music.ogg", 1.0f);

		// Textures are loaded
		debug_tex = App->tex->Load("maps/path2.png");
		gui_tex = App->tex->Load("gui/atlas.png");

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creating UI
		SDL_Rect section = { 537, 0, 663, 712 };
		settings_window = App->gui->CreateBox(&scene1Boxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, section, gui_tex);
		settings_window->visible = false;

		SDL_Rect idle = { 0, 391, 84, 49 };
		SDL_Rect hovered = { 0, 293, 84, 49 };
		SDL_Rect clicked = { 0, 342, 84, 49 };
		App->gui->CreateButton(&scene1Buttons, BUTTON, 31, 105, idle, hovered, clicked, gui_tex, SAVE_GAME, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateButton(&scene1Buttons, BUTTON, 78, 105, idle, hovered, clicked, gui_tex, CLOSE_GAME, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateBox(&scene1Boxes, BOX, App->gui->lastSlider1X, App->gui->slider1Y, { 416, 72, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, App->gui->minimum, App->gui->maximum);
		App->gui->CreateBox(&scene1Boxes, BOX, App->gui->lastSlider2X, App->gui->slider2Y, { 416, 72, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, App->gui->minimum, App->gui->maximum);

		SDL_Rect idle2 = { 28, 201, 49, 49 };
		SDL_Rect hovered2 = { 77, 201, 49, 49 };
		SDL_Rect clicked2 = { 126, 201, 49, 49 };
		App->gui->CreateButton(&scene1Buttons, BUTTON, 63, 135, idle2, hovered2, clicked2, gui_tex, CLOSE_SETTINGS, (j1UserInterfaceElement*)settings_window);

		SDL_Rect idle4 = { 417, 292, 49, 49 };
		SDL_Rect hovered4 = { 417, 345, 49, 49 };
		SDL_Rect clicked4 = { 417, 400, 49, 49 };
		App->gui->CreateButton(&scene1Buttons, BUTTON, 37, 135, idle4, hovered4, clicked4, gui_tex, GO_TO_MENU, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateLabel(&scene1Labels, LABEL, 44, 9, font, "Settings", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 30, 50, font, "Sound", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 30, 89, font, "Music", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 38, 143, font, "Menu", App->gui->grey, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 33, 110, font, "Save", App->gui->beige, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 81, 110, font, "Quit", App->gui->beige, (j1UserInterfaceElement*)settings_window);

		startup_time.Start();
	}

	return true;
}

// Called each loop iteration
bool j1Scene1::PreUpdate()
{
	BROFILER_CATEGORY("Level1PreUpdate", Profiler::Color::Orange)

	return true;
}

// Called each loop iteration
bool j1Scene1::Update(float dt)
{
	BROFILER_CATEGORY("Level1Update", Profiler::Color::LightSeaGreen)

	time_scene1 = startup_time.ReadSec();

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------		

	App->gui->UpdateButtonsState(&scene1Buttons);
	App->gui->UpdateWindow(settings_window, &scene1Buttons, &scene1Labels, &scene1Boxes);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || closeSettings) {
		settings_window->visible = !settings_window->visible;
		App->gamePaused = !App->gamePaused;

		if (App->render->camera.x != 0 && App->render->camera.x > App->entity->player->cameraLimit)
			settings_window->position = { (int)App->entity->player->position.x - App->gui->settingsPosition.x, App->gui->settingsPosition.y };
		else
			settings_window->position.x = App->gui->settingsPosition.x - App->render->camera.x / 4;

		for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next) {
			if (item->data->parent == settings_window) {
				item->data->visible = !item->data->visible;
				item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
				item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
			}
		}
		for (p2List_item<j1Label*>* item = scene1Labels.start; item != nullptr; item = item->next) {
			if (item->data->parent == settings_window) {
				item->data->visible = !item->data->visible;
				item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
				item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
			}
		}
		for (p2List_item<j1Box*>* item = scene1Boxes.start; item != nullptr; item = item->next) {
			if (item->data->parent == settings_window) {
				item->data->visible = !item->data->visible;
				item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
				item->data->position.y = settings_window->position.y + item->data->initialPosition.y;

				item->data->minimum = item->data->originalMinimum + settings_window->position.x;
				item->data->maximum = item->data->originalMaximum + settings_window->position.x;

				item->data->distanceCalculated = false;
			}
		}

		if (!settings_window->visible) closeSettings = false;
	}

	App->gui->UpdateSliders(&scene1Boxes);

	// Button actions
	for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next) {
		switch (item->data->state)
		{
		case IDLE:
			item->data->situation = item->data->idle;
			break;

		case HOVERED:
			item->data->situation = item->data->hovered;
			break;

		case RELEASED:
			item->data->situation = item->data->idle;
			if (item->data->bfunction == GO_TO_MENU) {
				backToMenu = true;
				App->gamePaused = false;
				settings_window->visible = false;
				App->fade->FadeToBlack();
			}
			else if (item->data->bfunction == CLOSE_SETTINGS) {
				closeSettings = true;
			}
			else if (item->data->bfunction == SAVE_GAME) {
				App->SaveGame("save_game.xml");
			}
			else if (item->data->bfunction == CLOSE_GAME) {
				continueGame = false;
			}
			break;

		case CLICKED:
			item->data->situation = item->data->clicked;
			break;
		}
	}

	// Load and Save
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	// Managing scene transitions
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN || resettingLevel)
	{
		resettingLevel = true;
		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0) {
			App->entity->player->position = initialScene1Position;
			App->render->camera.x = 0;
			App->entity->player->facingRight = true;
			resettingLevel = false;
		}
	}

	if (backToMenu && App->fade->IsFading() == 0)
		ChangeSceneMenu();

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	App->map->Draw();	

	return true;
}

// Called each loop iteration
bool j1Scene1::PostUpdate()
{
	BROFILER_CATEGORY("Level1PostUpdate", Profiler::Color::Yellow)

	return continueGame;
}

bool j1Scene1::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	return true;
}

bool j1Scene1::Save(pugi::xml_node& node) const
{
	pugi::xml_node activated = node.append_child("activated");

	activated.append_attribute("true") = active;

	return true;
}

// Called before quitting
bool j1Scene1::CleanUp()
{
	LOG("Freeing scene");
	App->tex->UnLoad(gui_tex);
	App->tex->UnLoad(debug_tex);

	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->gui->CleanUp();

	if (App->entity->player)
		App->entity->player->CleanUp();

	for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene1Buttons.del(item);
	}

	for (p2List_item<j1Label*>* item = scene1Labels.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene1Labels.del(item);
	}

	for (p2List_item<j1Box*>* item = scene1Boxes.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene1Boxes.del(item);
	}

	delete settings_window;
	if (settings_window != nullptr) settings_window = nullptr;

	return true;
}

void j1Scene1::ChangeSceneMenu()
{
	App->scene1->active = false;
	App->menu->active = true;

	CleanUp();
	App->fade->FadeToBlack();
	App->entity->CleanUp();
	App->entity->active = false;
	App->menu->Start();
	App->render->camera = { 0,0 };
	backToMenu = false;
}
