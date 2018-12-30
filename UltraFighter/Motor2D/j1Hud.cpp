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
	sprites = nullptr;
	
}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	text = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

	sprites = App->tex->Load("textures/coin.png");
	lives_tex = App->tex->Load("textures/life.png");

	seconds = App->gui->CreateLabel(&labels_list, LABEL, 540, 0, text, time_text.GetString());
	minutes = App->gui->CreateLabel(&labels_list, LABEL, 450, 0, text, "00:");


	if (App->scene1->active)
		coins_label = App->gui->CreateLabel(&labels_list, LABEL, 80, 700, text, score.GetString(), { 255, 255, 255, 255 });

	score_label = App->gui->CreateLabel(&labels_list, LABEL, 900, 0, text, score_points.GetString(), { 236, 151, 0, 255 });

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
	
	App->tex->UnLoad(seconds->sprites);
	seconds->sprites = App->font->Print(time_text.GetString(), seconds->color, seconds->font);

	if (seconds->sprites != nullptr)
		seconds->Draw(1.0f, 0, 0, false);
	if (minutes->sprites != nullptr)
		minutes->Draw(1.0f, 0, 0, false);
	if (App->gui->debug)
		App->render->DrawQuad({ 450, 0, 160, 64 }, 255, 0, 0, 255, false, false);

	//COIN
	SDL_Rect r = animation->GetCurrentFrame(dt);
	App->render->Blit(sprites, 3, 700, &r, SDL_FLIP_NONE, 1.0f, 1, 0, INT_MAX, INT_MAX, false);
	if (App->gui->debug)
		App->render->DrawQuad({ 3, 700, r.w*4, r.h*4 }, 255, 0, 0, 255, false, false);

	//PLAYER LIVES
	int space = 0;

	//LABELS
	if (coins_label != nullptr)
	{
		App->tex->UnLoad(coins_label->sprites);
		coins_label->sprites = App->font->Print(score.GetString(), coins_label->color, coins_label->font);
		if (coins_label->sprites != nullptr)
			coins_label->Draw(1.0f, 0, 0, false);

		if (App->gui->debug)
			App->render->DrawQuad({ 80, 700, 64, 64 }, 255, 0, 0, 255, false, false);
	}

	if (score_label != nullptr)
	{
		App->tex->UnLoad(score_label->sprites);
		score_label->sprites = App->font->Print(score_points.GetString(), score_label->color, score_label->font);
		if (score_label->sprites != nullptr)
			score_label->Draw(1.0f, 0, 0, false);
		if (App->gui->debug)
			App->render->DrawQuad({ 900, 0, 124, 64 }, 255, 0, 0, 255, false, false);
	}

	return true;
}

bool j1Hud::CleanUp()
{
	for (p2List_item<j1Label*>* item = labels_list.start; item != nullptr; item = item->next) 
	{
		labels_list.del(item);
	}

	App->tex->UnLoad(sprites);
	App->tex->UnLoad(lives_tex);

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
