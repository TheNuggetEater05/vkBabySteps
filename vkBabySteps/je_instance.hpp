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

		JeInstance(const JeInstance&) = delete;
		JeInstance& operator=(const JeInstance&) = delete;

		VkInstance get() const { return instance; };

	private:
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

		void createInstance(const char* appName);
		bool checkValidationLayerSupport() const;
		void createDebugMessenger();
	};
}