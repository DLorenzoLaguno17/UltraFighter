#include "j1Hud.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1UserInterfaceElement.h"
#include "j1Label.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Input.h"

j1Hud::j1Hud() 
{
	animation = NULL;
	
}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	text = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);
	Ryu = App->tex->Load("gui/Ui.png");
	Chunli = App->tex->Load("gui/Ui.png");
	vs = App->tex->Load("gui/Ui.png");
	Ryulifegood = App->tex->Load("gui/Ui.png");
	Ryulifebad = App->tex->Load("gui/Ui.png");
	Chunlilifegood = App->tex->Load("gui/Ui.png");
	Chunlilifebad = App->tex->Load("gui/Ui.png");
	seconds = App->gui->CreateLabel(&labels_list, LABEL, 530, 70, text, time_text.GetString());
	minutes = App->gui->CreateLabel(&labels_list, LABEL, 400, 70, text, "00:");
	C_lifepoints = 420;
	R_lifepoints = 420;
	R_PointsToSubstract = 0;
	C_PointsToSubstract = 0;

	animation = &idle;

	return true;
}

bool j1Hud::Update(float dt)
{
	//TIMER
	if (App->scene1->active)
	{
		time_text = { "%i", App->scene1->time_scene1 };
		if (App->scene1->time_scene1 == 60)
		{
			min += 1;
			App->tex->UnLoad(minutes->sprites);
			App->scene1->startup_time.Start();
			time_text = { "%i", App->scene1->time_scene1 };
			if (min < 10)
			{
				min_text_left.Clear();
				min_text = { "%i", min };
				min_text_left.operator+=("0");
				min_text_left.operator+=(min_text);
				min_text_left.operator+=(":");
				minutes->sprites = App->font->Print(min_text_left.GetString(), minutes->color, minutes->font);
			}
			else
			{
				min_text = { "%i", min };
				min_text.operator+=(":");
				minutes->sprites = App->font->Print(min_text.GetString(), minutes->color, minutes->font);
			}
		}
	}
	
	if (R_PointsToSubstract > 0) {
		R_lifepoints--;
		R_PointsToSubstract--;
	}
	if (C_PointsToSubstract > 0) {
		C_lifepoints--;
		C_PointsToSubstract--;
	}

	App->tex->UnLoad(seconds->sprites);
	seconds->sprites = App->font->Print(time_text.GetString(), seconds->color, seconds->font);

	if (seconds->sprites != nullptr)
		seconds->Draw(2.0f, 0, 0, false);
	if (minutes->sprites != nullptr)
		minutes->Draw(2.0f, 0, 0, false);
	if (App->gui->debug)
		App->render->DrawQuad({ 450, 0, 160, 64 }, 255, 0, 0, 255, false, false);


	SDL_Rect c = { 0, 854 ,436,117 };
	SDL_Rect r = { 0, 971 ,434,133 };
	SDL_Rect v = { 0, 1105 ,70,44 };
	SDL_Rect liferyu = { 465, 843 ,R_lifepoints,11 };
	SDL_Rect lifebad = { 0, 842 ,420,11 };
	SDL_Rect lifechunli = { 465, 843 ,C_lifepoints,11 };
	App->render->Blit(Ryu, 20, 20, &r , SDL_FLIP_NONE, 1.0f, 0.35f, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(Chunli, 540, 20, &c, SDL_FLIP_NONE, 1.0f, 0.35f, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(vs, 470, 40, &v , SDL_FLIP_NONE, 1.0f, 0.35f, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(Ryulifegood, 20, 5, &lifebad, SDL_FLIP_NONE, 1.0f, 0.35f, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(Ryulifebad, 20, 5, &lifechunli, SDL_FLIP_NONE, 1.0f, 0.35f, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(Chunlilifegood, 555, 5, &lifebad, SDL_FLIP_NONE, 1.0f, 0.35f, 0, INT_MAX, INT_MAX, false);
	App->render->Blit(Chunlilifebad, 555, 5, &lifechunli, SDL_FLIP_NONE, 1.0f, 0.35f, 0, INT_MAX, INT_MAX, false);
	


	return true;
}

bool j1Hud::CleanUp()
{
	for (p2List_item<j1Label*>* item = labels_list.start; item != nullptr; item = item->next) 
	{
		labels_list.del(item);
	}

	App->tex->UnLoad(Ryu);
	App->tex->UnLoad(Chunli);
	App->tex->UnLoad(vs);
	App->tex->UnLoad(Ryulifegood);
	App->tex->UnLoad(Ryulifebad);
	App->tex->UnLoad(Chunlilifegood);
	App->tex->UnLoad(Chunlilifebad);

	return true;
}

bool j1Hud::Load(pugi::xml_node & data)
{
	pugi::xml_node hud = data.child("player").child("hud");

	return true;
}

bool j1Hud::Save(pugi::xml_node & data) const
{
	pugi::xml_node hud = data;

	return true;
}
