#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace je
{
	class JeWindow
	{
	public:
		JeWindow(const char* title, uint32_t w, uint32_t h);
		~JeWindow();

		JeWindow(const JeWindow&) = delete;
		JeWindow& operator=(const JeWindow&) = delete;

		SDL_Window* nativeWindow() { return window; };
		const SDL_Window* nativeWindow() const { return window; };

	private:
		SDL_Window* window;

		void createWindow(const char* title, uint32_t w, uint32_t h);
	};
}