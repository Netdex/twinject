//Using SDL and standard IO
#include <SDL.h>
#include <cassert>
#include <util/vec2.h>
#include "scene.h"
#include "model/object.h"
#include <ctime>
#include <iostream>
#include <string>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

static float randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void close()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	SDL_Quit();
}

std::shared_ptr<obb> randomobb()
{
	return std::make_shared<obb>(
		vec2(randf() * SCREEN_WIDTH, randf() * SCREEN_HEIGHT),
		randf() * 400 + 30,
		randf() * 200 + 30,
		randf() * 7,
		vec2(randf() * 4 - 2, randf() * 4 - 2)
	);
}

scene sc;

void initScene()
{
	sc = scene();
	sc.entities.push_back(
		std::make_shared<circle>(vec2(50, 50), vec2(1, 1), 50));
	sc.entities.push_back(
		std::make_shared<circle>(vec2(1000, 50), vec2(-1, .7), 150));
}

void loop()
{
	bool quit = false;
	SDL_Event e;

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if(e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_r:
					initScene();
					break;
				}
			}
		}

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);

		sc.render(gRenderer);
		sc.tick();

		SDL_RenderPresent(gRenderer);
	}
}
int main(int argc, char* args[])
{
	srand(static_cast <unsigned> (time(0)));

	gWindow = SDL_CreateWindow("Twinject Sandbox", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	assert(gWindow != NULL);

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert(gRenderer != NULL);

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	initScene();
	loop();
	close();

	return 0;
}