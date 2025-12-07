#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace je
{
	class JeWindow
	{
	public:
		JeWindow(const char* title, uint32_t w, uint32_t h);
		~JeWindow();

		JeWindow(const JeWindow&) = delete;
		JeWindow& operator=(const JeWindow&) = delete;

		SDL_Window* get() { return window; };
		const SDL_Window* get() const { return window; };

		VkSurfaceKHR getSurface() { return surface; };

		void createSurface(VkInstance instance);

	private:
		SDL_Window* window;
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		void createWindow(const char* title, uint32_t w, uint32_t h);
	};
}