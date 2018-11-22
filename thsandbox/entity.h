#pragma once
#include "util/vec2.h"
#include <SDL.h>

class entity
{
protected:
	entity(vec2 position, vec2 velocity) : position(position), velocity(velocity) {}
public:
	vec2 position;
	vec2 velocity;

	virtual ~entity() = default;
	virtual void render(SDL_Renderer *renderer) = 0;
};

class aabb : public entity
{
public:

	vec2 size;

	aabb(vec2 position, vec2 velocity, vec2 size) : entity(position, velocity), size(size) {}

	void render(SDL_Renderer *renderer) override
	{
		SDL_Rect fillRect = { position.x, position.y, size.x, size.y };
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderDrawRect(renderer, &fillRect);
	}
};

class circle : public entity
{
	float radius;
};
