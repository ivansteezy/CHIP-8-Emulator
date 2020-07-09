#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>

#include "cpu.h"

Chip8* myChip = new Chip8();

int main()
{
	Chip8* myChip = new Chip8();
	// this now load rom file into memory
	myChip->load("C:\\Users\\Iván\\Downloads\\TETRIS");

	int mustQuit = 0;
	int lastTicks = 0;

	//SDL shit
	SDL_Event ev;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* win = SDL_CreateWindow("CHIP-8 Emulator",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640, 320,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_Renderer* rnd = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture* tex = SDL_CreateTexture(rnd, SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		64, 32);

	SDL_Surface* surface = SDL_CreateRGBSurface(NULL, 64, 32, 32,
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000);
	SDL_LockTexture(tex, NULL, &surface->pixels, &surface->pitch);
	Expansion(myChip->gfx, (Uint32*)surface->pixels);
	SDL_UnlockTexture(tex);

	int cycles = 0;

	while (!mustQuit)
	{
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
			case SDL_QUIT:
				mustQuit = 1;
				break;
			}
		}

		if (SDL_GetTicks() - cycles > 1)
		{
			myChip->emulateCycle();
			cycles = SDL_GetTicks();
		}

		if (SDL_GetTicks() - lastTicks > (1000 / 60))
		{
			if (myChip->delay_timer) myChip->delay_timer--;
			if (myChip->sound_timer) myChip->sound_timer--;

			SDL_LockTexture(tex, NULL, &surface->pixels, &surface->pitch);
			Expansion(myChip->gfx, (Uint32*)surface->pixels);
			SDL_UnlockTexture(tex);

			SDL_RenderCopy(rnd, tex, NULL, NULL);
			SDL_RenderPresent(rnd);
			lastTicks = SDL_GetTicks();
		}
	}

	SDL_DestroyRenderer(rnd);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}