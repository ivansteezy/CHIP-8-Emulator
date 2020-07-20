#define SDL_MAIN_HANDLED
#include "Chip8.hpp"
#include "Renderer.hpp"

int main()
{
	auto myChip = std::make_shared<Machine::Chip8>();
	myChip->Load("../../Roms/Sierpinski.ch8");

	auto renderer = std::make_shared<Render::Chip8Window>("Chip", 640, 320, myChip);
	renderer->Begin();
	
	return 0;
}