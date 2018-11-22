#pragma once
#include "entity.h"
#include <SDL.h>

class scene
{
public:
	std::vector<entity*> entities;

	scene();
	~scene();

	void render(SDL_Renderer *renderer);
	void tick();
};

