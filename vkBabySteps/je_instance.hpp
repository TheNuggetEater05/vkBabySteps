#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace je
{
	class JeInstance
	{
	public:
		JeInstance(const char* appName, SDL_Window* window);
		~JeInstance();

		VkInstance get() const { return instance; };

	private:
		VkInstance instance;

		void createInstance(const char* appName, SDL_Window* window);
	};
}