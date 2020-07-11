#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <memory>
#include <string>
#include <array>
#include <SDL.h>

#include "Chip8.hpp"

namespace Render
{
	class Chip8Window
	{
	public:
		Chip8Window(const std::string& windowName, const int& width, const int& height, std::shared_ptr<Chip8>& chip);
		void Begin();

	private:
		void RenderWindow();
	public:
		static void Expansion(std::array<uint8_t, 64 * 32> from, uint32_t* to)
		{
			for (int i = 0; i < 2048; i++)
				to[i] = (from[i]) ? -1 : 0;
		}

	private:
		SDL_Event m_event;
		SDL_Window* m_window;
		SDL_Texture* m_texture;
		SDL_Surface* m_surface;
		SDL_Renderer* m_renderer;
		std::shared_ptr<Chip8> m_chip;
	};
}

#endif