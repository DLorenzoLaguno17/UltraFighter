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
		//Loading background
		graphics = App->tex->Load("maps/map.png");

		//Background
		background.x = 0;
		background.y = 0;
		background.w = 512;
		background.h = 224;

		//Sky
		sky.x = 0;
		sky.y = 613;
		sky.w = 691;
		sky.h = 105;

		//Grid on the left
		grid.x = 0;
		grid.y = 472;
		grid.w = 94;
		grid.h = 100;

		//Animations
		couple_left.PushBack({ 0,366,64,97 });
		couple_left.PushBack({ 66,366,64,97 });
		couple_left.speed = 0.1f;

		guys.PushBack({ 140,392,98, 71 });
		guys.PushBack({ 243,392,98, 71 });
		guys.speed = 0.1f;

		couple_right.PushBack({ 347,384,81,79 });
		couple_right.PushBack({ 432,384,81,79 });
		couple_right.speed = 0.1f;

		// The audio is played	
		App->audio->PlayMusic("audio/Streetfighter2/Sound/in_game_music.ogg", 1.0f);

		// Textures are loaded
		debug_tex = App->tex->Load("maps/path2.png");
		gui_tex = App->tex->Load("gui/Ui.png");

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creating UI
		SDL_Rect section = { 546, 0, 616, 712 };
		settings_window = App->gui->CreateBox(&scene1Boxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, section, gui_tex);
		settings_window->visible = false;

		SDL_Rect idle = { 0, 602, 404, 36 };
		SDL_Rect hovered = { 0, 639, 404, 36 };
		SDL_Rect clicked = { 0, 675, 404, 36 };
		App->gui->CreateButton(&scene1Buttons, BUTTON, 10, 65, idle, hovered, clicked, gui_tex, GO_TO_MENU, (j1UserInterfaceElement*)settings_window);
		
		SDL_Rect idle2 = { 0, 713, 338, 41 };
		SDL_Rect hovered2 = { 0, 796, 338, 41 };
		SDL_Rect clicked2 = { 0, 755, 338, 40 };
		App->gui->CreateButton(&scene1Buttons, BUTTON, 20, 40, idle2, hovered2, clicked2, gui_tex, CLOSE_SETTINGS, (j1UserInterfaceElement*)settings_window);

		SDL_Rect sliderTex = { 0, 532, 25, 66 };
		App->gui->CreateBox(&scene1Boxes, BUTTON, 70, 107, sliderTex, gui_tex, (j1UserInterfaceElement*)settings_window, 110, 226);

		SDL_Rect rect3 = { 0,842,420,11 };
		App->gui->CreateButton(&scene1Buttons, BUTTON, 8, 115, rect3, rect3, rect3, gui_tex, NO_FUNCTION, (j1UserInterfaceElement*)settings_window);
		


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

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || closeSettings) {
		settings_window->visible = !settings_window->visible;
		App->gamePaused = !App->gamePaused;

		settings_window->position.x = App->gui->settingsPosition.x - App->render->camera.x / 3;

		for (p2List_item<j1Button*>* item = scene1Buttons.start; item != nullptr; item = item->next) {
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
			break;

		case CLICKED:
			item->data->situation = item->data->clicked;
			break;
		}
	}

	if (App->gameFinished) {
		backToMenu = true;
		App->gamePaused = false;
		settings_window->visible = false;
		App->fade->FadeToBlack();
	}
	
	if (backToMenu && App->fade->IsFading() == 0)
	{
		App->SaveGame("save_game.xml");
		ChangeSceneMenu();
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	App->render->Blit(graphics, 0, 0, &sky);
	App->render->Blit(graphics, 0, 0, &background);
	App->render->Blit(graphics, 0, 65, &grid);
	App->render->Blit(graphics, 0, 115, &(couple_left.GetCurrentFrame(0.3f)));
	App->render->Blit(graphics, 100, 115, &(guys.GetCurrentFrame(0.2f)));
	App->render->Blit(graphics, 200, 115, &(couple_right.GetCurrentFrame(0.4f)));

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
