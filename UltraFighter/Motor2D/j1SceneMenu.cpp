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
		Ryu = App->tex->Load("gui/menufaces.png");
		Chunli = App->tex->Load("gui/menufaces.png");
		vs = App->tex->Load("gui/Ui.png");

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creating UI		

		// We will use it to check if there is a save file
		pugi::xml_document save_game;
		pugi::xml_parse_result result = save_game.load_file("save_game.xml");

		SDL_Rect idle = {0, 0, 406, 58};
		SDL_Rect hovered = { 0, 59, 406, 57 };
		SDL_Rect clicked = { 0, 117, 406, 58 };
		App->gui->CreateButton(&menuButtons, BUTTON, 190, 150, idle, hovered, clicked, gui_tex, PLAY_GAME);

		SDL_Rect idle2 = { 0, 176, 251, 58 };
		SDL_Rect hovered2 = { 0, 235, 251, 58 };
		SDL_Rect clicked2 = { 0, 293, 251, 58 };
		App->gui->CreateButton(&menuButtons, BUTTON, 190, 180, idle2, hovered2, clicked2, gui_tex, CLOSE_GAME);

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
				else if (item->data->bfunction == CLOSE_GAME) {
					continueGame = false;
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
	App->render->Blit(background, 0, 0, &rect, SDL_FLIP_NONE, 1.0f, 0.333333333333f);
	
	SDL_Rect c = { 325, 247 , 56 ,76 };
	SDL_Rect r = { 10, 246 ,54, 77 };
	SDL_Rect v = { 0, 1105 ,70,44 };
	App->render->Blit(Ryu, 500, 20, &r, SDL_FLIP_NONE, 1.0f, 1, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(Chunli, 820, 180, &c, SDL_FLIP_HORIZONTAL, 1.0f, 1, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(vs, 690, 190, &v, SDL_FLIP_NONE, 1.0f, 0.5f, 0, INT_MAX, INT_MAX, false);


	// Blitting the buttons and labels of the menu
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		item->data->Draw(App->gui->buttonsScale);
	}

	// Blitting settings window
	if (settings_window != nullptr && settings_window->visible == true)
		settings_window->Draw(App->gui->settingsWindowScale);
	

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