#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>

#include "Chip8.hpp"
#include "Renderer.hpp"

int main()
{
	auto myChip = std::make_shared<Chip8>();
	myChip->Load("C:\\Users\\Iván\\Downloads\\Sierpinski.ch8");

	auto renderer = std::make_shared<Render::Chip8Window>("Chip", 640, 320, myChip);
	renderer->Begin();
	
	return 0;
}