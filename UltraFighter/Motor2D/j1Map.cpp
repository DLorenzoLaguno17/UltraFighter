#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1Collisions.h"

j1Map::j1Map()
{
	
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

	/*//Box
	box.x = 884;
	box.y = 884;
	box.w = ;
	box.h = ;*/

	//Grid on the left
	grid.x = 0;
	grid.y = 472;
	grid.w = 94;
	grid.h = 100;

	//Animations
	couple_left.PushBack({ 0,366,65,97 });
	couple_left.PushBack({ 65,366,65,97 });
	couple_left.speed = 0.1f;

	guys.PushBack({ 130,382,108, 71 });
	guys.PushBack({ 238,382,108, 71 });
	guys.speed = 0.1f;

	couple_right.PushBack({ 0,382,83,81 });
	couple_right.PushBack({ 65,382,83,81 });
	couple_right.speed = 0.1f;


}

j1Map::~j1Map()
{

}

bool j1Map::Start()
{
	LOG("Loading background...");
	bool ret = true;

	graphics = App->tex->Load("map.png");

	return ret;
		
}

void j1Map::Draw()
{
	App->render->Blit(graphics, 0, 0, &background);
	App->render->Blit(graphics, 0, 0, &sky);

	App->render->Blit(graphics, 0, 0, &(couple_left.GetCurrentFrame(0.75f)));
	App->render->Blit(graphics, 0, 0, &(guys.GetCurrentFrame(0.75f)));
	App->render->Blit(graphics, 0, 0, &(couple_right.GetCurrentFrame(0.75f)));

}