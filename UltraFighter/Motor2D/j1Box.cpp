#include "j1Box.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Render.h"

j1Box::j1Box(UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text, j1UserInterfaceElement* parent, uint minimum, uint maximum) :
	j1UserInterfaceElement(UIELEMENT_TYPES::BOX, x, y, text, parent), section(section), originalMinimum(minimum), originalMaximum(maximum), minimum(minimum), maximum(maximum) {}

j1Box::~j1Box() {}

void j1Box::Draw(float scale, int x, int y, bool use_camera)
{
	if (!use_camera)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_NONE, 1.0f, scale, INT_MAX, INT_MAX, false);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &section, SDL_FLIP_NONE, 1.0f, scale);
}

bool j1Box::CleanUp()
{
	LOG("Unloading button textures");
	App->tex->UnLoad(sprites);

	return true;
}