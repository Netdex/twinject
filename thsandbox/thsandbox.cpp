//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <cassert>
#include <util/vec2.h>
#include "scene.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

void close()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	SDL_Quit();
}

scene sc;

void initScene()
{
	sc = scene();
	aabb *ab = new aabb(vec2(10, 10), vec2(1, 0.5), vec2(50, 60));
	sc.entities.push_back(ab);

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