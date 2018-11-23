#pragma once
#include <SDL.h>
#include <vector>
#include "model/object.h"

class scene
{
public:
	std::vector<std::shared_ptr<entity>> entities;

	scene();
	~scene();

	void render(SDL_Renderer *renderer);
	void tick();
};

