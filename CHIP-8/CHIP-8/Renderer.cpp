#include "Renderer.hpp"

Render::Chip8Window::Chip8Window(const std::string& windowName, const int& width, const int& height, std::shared_ptr<Chip8>& chip)
	: m_chip(chip)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	m_window = SDL_CreateWindow(windowName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		64, 32);

	m_surface = SDL_CreateRGBSurface(NULL, 64, 32, 32,
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000);

	SDL_LockTexture(m_texture, NULL, &m_surface->pixels, &m_surface->pitch);
	Expansion(m_chip->screen, (Uint32*)m_surface->pixels);
	SDL_UnlockTexture(m_texture);
}

void Render::Chip8Window::Begin()
{
	int mustQuit = 0;
	int cycles = 0;
	
	while (!mustQuit)
	{
		while (SDL_PollEvent(&m_event))
		{
			switch (m_event.type)
			{
			case SDL_QUIT: mustQuit = 1; break;
			}
		

			if (SDL_GetTicks() - cycles > 1)
			{
				m_chip->EmulateCycle();
				cycles = SDL_GetTicks();
			}

			RenderWindow();
		}
	}

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Render::Chip8Window::RenderWindow()
{
	int lastTicks = 0;
	if (SDL_GetTicks() - lastTicks > (1000 / 60))
	{
		if (m_chip->delayTimer) m_chip->delayTimer--;
		if (m_chip->soundTimer) m_chip->soundTimer--;

		SDL_LockTexture(m_texture, NULL, &m_surface->pixels, &m_surface->pitch);
		Expansion(m_chip->screen, (Uint32*)m_surface->pixels);
		SDL_UnlockTexture(m_texture);

		SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
		SDL_RenderPresent(m_renderer);
		lastTicks = SDL_GetTicks();
	}
}
