#include "je_window.hpp"

namespace je
{
	JeWindow::JeWindow(const char* title, uint32_t w, uint32_t h)
	{
		createWindow(title, w, h);
	}

	JeWindow::~JeWindow()
	{
		SDL_DestroyWindow(window);
	}

	void JeWindow::createWindow(const char* title, uint32_t w, uint32_t h)
	{
		window = SDL_CreateWindow(title, static_cast<int>(w), static_cast<int>(h), SDL_WINDOW_VULKAN);
	}
}