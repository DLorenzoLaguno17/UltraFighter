#include "j1Label.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "p2Log.h"

#include "Brofiler/Brofiler.h"

j1Label::j1Label(UIELEMENT_TYPES type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color, j1UserInterfaceElement* parent) :
	j1UserInterfaceElement(UIELEMENT_TYPES::LABEL, x, y, App->font->Print(text, color, font), parent), font(font), text(text), color(color) {}

j1Label::~j1Label() {}

void j1Label::Draw(float scale, int x, int y, bool use_camera)
{
	if (!use_camera)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, NULL, SDL_FLIP_NONE, 1.0f, scale, 0, INT_MAX, INT_MAX, false);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, NULL, SDL_FLIP_NONE, 1.0f, scale);
}

bool j1Label::CleanUp()
{
	LOG("Unloading label textures");
	App->tex->UnLoad(sprites);

	return true;
}