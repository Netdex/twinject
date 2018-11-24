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
	/*sc.entities.push_back(
		std::make_shared<aabb>(vec2(10, 10), vec2(2, 0), vec2(50, 60)));
	sc.entities.push_back(
		std::make_shared<aabb>(vec2(1000, 10), vec2(-2, 0), vec2(150, 260)));*/
	/*sc.entities.push_back(
		std::make_shared<obb>(vec2(70, 10), 100, 30, 10, vec2(1,2)));*/
	/*sc.entities.push_back(
		std::make_shared<obb>(vec2(10, 300), 10, 100, 2, vec2(1, 0.1)));
	sc.entities.push_back(
		std::make_shared<obb>(vec2(1000, 300), 10, 50, 1, vec2(-1, 0.1)));*/
	/*sc.entities.push_back(
		std::make_shared<obb>(vec2(10, 150), 50, 50, 0, vec2(2, 0)));
	sc.entities.push_back(
		std::make_shared<obb>(vec2(1000, 150), 50, 50, 0, vec2(-2, 0)));*/

	sc.entities.push_back(randomobb());
	sc.entities.push_back(randomobb());

	for(const auto& e : sc.entities)
	{
		std::cout << e->serialize() << std::endl;
	}
	/*
	 * polygon { <239.496,194.975> <250.987,259.037> <154.489,276.346> <142.999,212.284> } v<-1.07639,0.227607>
polygon { <3.47287,267.613> <147.079,395.19> <72.5695,479.06> <-71.0362,351.483> } v<1.5443,-1.15915>
	 */
	/*sc.entities.push_back(
		std::make_shared<polygon>(std::vector<vec2>{
			vec2(239.496, 194.975),
			vec2(250.987, 259.037),
			vec2(154.489, 276.346),
			vec2(142.999, 212.284)
		}, vec2(-1.07639, 0.227607))
	);

	sc.entities.push_back(
		std::make_shared<polygon>(std::vector<vec2>{
		vec2(3.47287, 267.613),
			vec2(147.079, 395.19),
			vec2(72.5695, 479.06),
			vec2(-71.0362, 351.483)
		}, vec2(1.5443, -1.15915))
	);*/
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