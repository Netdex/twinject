#include "scene.h"



scene::scene()
{
}


scene::~scene()
{
	for (entity *e : entities)
		delete e;
}

void scene::render(SDL_Renderer *renderer)
{
	for (entity* e : entities)
		e->render(renderer);
}

void scene::tick()
{
	for (entity* e : entities)
		e->position += e->velocity;
}
