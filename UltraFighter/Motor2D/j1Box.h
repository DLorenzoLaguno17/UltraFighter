#ifndef __j1BOX_H__
#define __j1BOX_H__

#include "j1UserInterfaceElement.h"
#include "p2Defs.h"

enum UIELEMENT_TYPES;
struct SDL_Texture;

class j1Box : public j1UserInterfaceElement {
public:

	// Constructor
	j1Box(UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text = nullptr, j1UserInterfaceElement* parent = nullptr, uint minimum = 0, uint maximum = 0);

	// Destructor
	virtual ~j1Box();

	// Called before quitting
	bool CleanUp();

	// Blits the element on the screen
	void Draw(float scale = 1.0f, int x = 0, int y = 0, bool use_camera = true) override;

public:
	SDL_Rect section;
	bool clicked = false;

	bool distanceCalculated = false;
	iPoint mouseDistance;

	// They are used in case you want to limit the movement of the box
	uint originalMinimum;
	uint originalMaximum;
	uint minimum;
	uint maximum;
};

#endif // __j1BOX_H__