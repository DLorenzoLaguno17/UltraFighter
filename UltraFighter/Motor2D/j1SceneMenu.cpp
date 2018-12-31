#include "j1SceneMenu.h"
#include "j1Scene1.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1EntityManager.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "j1Gui.h"
#include "j1Button.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Window.h"

#include "Brofiler/Brofiler.h"

j1SceneMenu::j1SceneMenu()
{
	name.create("menu");

	player.LoadAnimations("idle");
}

j1SceneMenu::~j1SceneMenu() {}

bool j1SceneMenu::Awake(pugi::xml_node &)
{
	LOG("Loading Menu");
	bool ret = true;

	if (App->menu->active == false)
	{
		LOG("Unable to load the menu.");
		ret = false;
	}

	return ret;
}

bool j1SceneMenu::Start()
{	
	if (active)
	{
		// The map is loaded
		//App->map->Draw();

		// The audio is played
		App->audio->PlayMusic("audio/streetfighter2/Sound/menu_music.ogg", 1.0f);

		// Loading textures
		gui_tex = App->tex->Load("gui/Ui.png");
		player_tex = App->tex->Load("textures/character/character.png");
		harpy_tex = App->tex->Load("textures/enemies/harpy/harpy.png");
		background = App->tex->Load("gui/MenuBackground.png");

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creating UI		
		settings_window = App->gui->CreateBox(&menuBoxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, { 537, 0, 663, 712 }, gui_tex);
		settings_window->visible = false;

		// We will use it to check if there is a save file
		pugi::xml_document save_game;
		pugi::xml_parse_result result = save_game.load_file("save_game.xml");

		App->gui->CreateBox(&menuBoxes, BOX, App->gui->lastSlider1X, App->gui->slider1Y, { 0, 532, 25, 66 }, gui_tex, (j1UserInterfaceElement*)settings_window, App->gui->minimum, App->gui->maximum);
		App->gui->CreateBox(&menuBoxes, BOX, App->gui->lastSlider2X, App->gui->slider2Y, { 0, 532, 25, 66 }, gui_tex, (j1UserInterfaceElement*)settings_window, App->gui->minimum, App->gui->maximum);

		SDL_Rect idle = {0, 0, 406, 58};
		SDL_Rect hovered = { 0, 59, 406, 57 };
		SDL_Rect clicked = { 0, 117, 406, 58 };
		App->gui->CreateButton(&menuButtons, BUTTON, 170, 120, idle, hovered, clicked, gui_tex, PLAY_GAME);

		SDL_Rect idle2 = { 0, 176, 251, 58 };
		SDL_Rect hovered2 = { 0, 235, 251, 58 };
		SDL_Rect clicked2 = { 0, 293, 251, 58 };
		App->gui->CreateButton(&menuButtons, BUTTON, 170, 150, idle2, hovered2, clicked2, gui_tex, CLOSE_GAME);

		//App->gui->CreateButton(&menuButtons, BUTTON, 64, 135, idle2, hovered2, clicked2, gui_tex, CLOSE_SETTINGS, (j1UserInterfaceElement*)settings_window);

		/*SDL_Rect idle3 = { 463, 109, 49, 49 };
		SDL_Rect hovered3 = { 463, 158, 49, 49 };
		SDL_Rect clicked3 = { 463, 207, 49, 49 };
		App->gui->CreateButton(&menuButtons, BUTTON, 3, 3, idle3, hovered3, clicked3, gui_tex, SETTINGS);

		App->gui->CreateLabel(&menuLabels, LABEL, 106, 115, font, "Start", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 98, 165, font, "Credits", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 90, 140, font, "Continue", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 44, 9, font, "Settings", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&menuLabels, LABEL, 30, 50, font, "Sound", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&menuLabels, LABEL, 30, 89, font, "Music", App->gui->brown, (j1UserInterfaceElement*)settings_window);*/

		player_created = false;
	}

	return true;
}

bool j1SceneMenu::PreUpdate()
{
	BROFILER_CATEGORY("MenuPreUpdate", Profiler::Color::Orange)
	return true;
}

bool j1SceneMenu::Update(float dt)
{
	BROFILER_CATEGORY("MenuUpdate", Profiler::Color::LightSeaGreen)

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------	

	// Updating the state of the UI
	App->gui->UpdateButtonsState(&menuButtons); 
	App->gui->UpdateSliders(&menuBoxes);
	App->gui->UpdateWindow(settings_window, &menuButtons, &menuLabels, &menuBoxes);

	// Button actions
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->visible) {
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
				if (item->data->bfunction == PLAY_GAME) {
					startGame = true;
					App->fade->FadeToBlack();
				}
				else if (item->data->bfunction == LOAD_GAME) {
					loadGame = true;
					App->fade->FadeToBlack();
				}
				else if (item->data->bfunction == CLOSE_GAME) {
					continueGame = false;
				}
				else 
				if ((item->data->bfunction == SETTINGS && !settings_window->visible) 
					|| (item->data->bfunction == CLOSE_SETTINGS && settings_window->visible)) {
					settings_window->visible = !settings_window->visible;
					settings_window->position = App->gui->settingsPosition;

					for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
						if (item->data->parent == settings_window) {
							item->data->visible = !item->data->visible;
							item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
							item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
						}
					}
					for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
						if (item->data->parent == settings_window) {
							item->data->visible = !item->data->visible;
							item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
							item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
						}
					}
					for (p2List_item<j1Box*>* item = menuBoxes.start; item != nullptr; item = item->next) {
						if (item->data->parent == settings_window) {
							item->data->visible = !item->data->visible;
							item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
							item->data->position.y = settings_window->position.y + item->data->initialPosition.y;

							item->data->minimum = item->data->originalMinimum + settings_window->position.x;
							item->data->maximum = item->data->originalMaximum + settings_window->position.x;
						}
					}
				}
				else if (item->data->bfunction == OPEN_CREDITS) {
					openCredits = true;
					App->fade->FadeToBlack();
				}
				break;

			case CLICKED:
				item->data->situation = item->data->clicked;
				break;
			}
		}
	}

	// Managing scene transitions
	if (App->fade->IsFading() == 0) {
		if (startGame) {
			ChangeScene(SCENE1);
			player_created = true;
		}
		else if (loadGame)
			App->LoadGame("save_game.xml");
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	SDL_Rect rect = { 0,0,1024,768 };
	App->render->Blit(background, 0, 0, &rect, SDL_FLIP_NONE, 1.0f, 0.333333333333);

	SDL_Rect rect2 = { 0,352,507,58 };
	App->render->Blit(gui_tex, 170, 180, &rect2, SDL_FLIP_NONE, 1.0f, App->gui->buttonsScale);

	SDL_Rect rect3 = { 0,842,420,11 };
	App->render->Blit(gui_tex, 170, 210, &rect3, SDL_FLIP_NONE, 1.0f, App->gui->buttonsScale);

	// Blitting the buttons and labels of the menu
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		item->data->Draw(App->gui->buttonsScale);
	}
	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		if (item->data->visible) item->data->Draw();
	}
	

	// Blitting settings window
	if (settings_window != nullptr && settings_window->visible == true)
		settings_window->Draw(App->gui->settingsWindowScale);

	// Blitting the buttons, labels and boxes (sliders) of the window
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else
			item->data->Draw(App->gui->buttonsScale);
		
	}

	for (p2List_item<j1Box*>* item = menuBoxes.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else
			item->data->Draw(App->gui->buttonsScale);
	}

	return true;
}

bool j1SceneMenu::PostUpdate()
{
	BROFILER_CATEGORY("MenuPostUpdate", Profiler::Color::Yellow)

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		continueGame = false;

	return continueGame;
}

bool j1SceneMenu::CleanUp()
{
	LOG("Freeing all textures");
	App->tex->UnLoad(harpy_tex);
	App->tex->UnLoad(player_tex);
	App->tex->UnLoad(gui_tex);
	
	App->map->CleanUp();
	App->tex->CleanUp();

	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		menuButtons.del(item);
	}

	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		menuLabels.del(item);
	}

	for (p2List_item<j1Box*>* item = menuBoxes.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		menuBoxes.del(item);
	}

	delete settings_window;
	if(settings_window != nullptr) settings_window = nullptr;

	return true;
}

bool j1SceneMenu::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	if ((scene_activated == false) && active)
		ChangeScene(SCENE1);

	return true;
}

void j1SceneMenu::ChangeScene(SCENE objectiveScene)
{
	if (!player_created)
	{
		this->active = false;
		startGame = false;
		loadGame = false;
		openCredits = false;

		CleanUp();

		App->scene1->active = true;
		App->scene1->Start();			
		App->entity->active = true;
		App->entity->CreatePlayer();
		App->entity->Start();
		
	}
}