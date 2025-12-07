#include "je_instance.hpp"

#include <stdexcept>
#include <vector>

namespace je
{
	JeInstance::JeInstance(const char* appName, SDL_Window* window)
	{
		createInstance(appName, window);
	}

	JeInstance::~JeInstance()
	{

	}

	void JeInstance::createInstance(const char* appName, SDL_Window* window)
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t availableExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

		SDL_Log("Available extensions: ");
		for (int i = 0; i < availableExtensions.size(); i++)
		{
			SDL_Log("\tExtension %u: %s", i, availableExtensions[i].extensionName);
		}

		// Get SDL's required extensions

		uint32_t sdlExtensionCount = 0;
		const char* const* sdlExtensions;
		if (!(sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount)))
			throw std::runtime_error("Failed to get SDL extension count");

		std::vector<const char*> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		SDL_Log("Using extensions: ");
		for (int i = 0; i < extensions.size(); i++)
		{
			SDL_Log("\tExtension %u: %s", i, extensions[i]);
		}

		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vulkan instance");
	}
}