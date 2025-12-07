#include "je_instance.hpp"

#include <stdexcept>
#include <vector>

#ifdef _DEBUG
constexpr bool validationLayersEnabled = true;
const std::vector<const char*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};
#else
constexpr bool validationLayersEnabled = false;
const std::vector<const char*> validationLayers = {};
#endif

namespace je
{
	JeInstance::JeInstance(const char* appName, SDL_Window* window)
	{
		createInstance(appName);
	}

	JeInstance::~JeInstance()
	{

	}

	void JeInstance::createInstance(const char* appName)
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

		if (validationLayersEnabled)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

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

	bool JeInstance::checkValidationLayerSupport() const
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> layers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		for (auto layerName : validationLayers)
		{
			bool found = false;
			for (const auto& layer : layers)
			{
				if (!strcmp(layerName, layer.layerName))
				{
					found = true;
					break;
				}
			}
			if (!found)
				return false;
		}
		return true;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
	{
		SDL_Log("VALIDATION LAYER: %s", callbackData->pMessage);
		return VK_FALSE;
	}

	void JeInstance::createDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		debugInfo.pfnUserCallback = debugCallback;

		auto createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (createDebugUtilsMessengerEXT)
			createDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessenger);
	}
}