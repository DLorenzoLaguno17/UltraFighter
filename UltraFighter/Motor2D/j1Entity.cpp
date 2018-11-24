#include "j1Entity.h"
#include "j1App.h"
#include "j1Render.h"

j1Entity::j1Entity(int x, int y, ENTITY_TYPES type) : position(x,y), type(type)
{
}

void j1Entity::Draw(SDL_Rect r, bool flip, int x, int y)
{
	if (flip == false)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_NONE);
	else 
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_HORIZONTAL);
}