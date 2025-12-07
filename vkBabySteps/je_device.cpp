#include "je_device.hpp"

#include <stdexcept>
#include <vector>
#include <SDL3/SDL_log.h>

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
	JeDevice::JeDevice(const VkInstance& instance, VkSurfaceKHR surfaceKHR) : surface(surfaceKHR)
	{
		getPhysicalDevice(instance);
		createLogicalDevice();
	}

	JeDevice::~JeDevice()
	{
		vkDestroyDevice(device, nullptr);
	}

	void JeDevice::getPhysicalDevice(const VkInstance& instance)
	{
		uint32_t physicalDeviceCount = 0;
		if (vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr) != VK_SUCCESS)
			throw std::runtime_error("Failed to enumerate physical devices");

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		if (vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to get vector of physical devices");

		SDL_Log("Choosing first discrete gpu found, falling back to integrated if none");

		VkPhysicalDevice integratedGPU = nullptr;
		for (int i = 0; i < physicalDeviceCount; i++)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties{};
			vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);

			SDL_Log("GPU %u: %s : %s", i, physicalDeviceProperties.deviceName, getDeviceType(physicalDeviceProperties.deviceType));

			if (physicalDeviceProperties.deviceType == 2)
			{
				SDL_Log("Discrete GPU selected: %s", physicalDeviceProperties.deviceName);
				physicalDevice = physicalDevices[i];
				break;
			}

			if (physicalDeviceProperties.deviceType == 1)
			{
				integratedGPU = physicalDevices[i];
			}
		}

		if (physicalDevice == nullptr)
		{
			SDL_Log("Failed to find discrete GPU, falling back to integrated");
			physicalDevice = integratedGPU;
		}
	}
	QueueFamilyIndices JeDevice::findQueueFamilies()
	{
		QueueFamilyIndices indices;

		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

		std::vector<VkQueueFamilyProperties> families(count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, families.data());

		for (int i = 0; i < count; i++)
		{
			if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
			if (presentSupport)
			{
				indices.presentFamily = i;
				logSurfaceInfo();
			}

			if (indices.isComplete())
				break;
		}

		return indices;
	}
	void JeDevice::retrieveQueues()
	{
		QueueFamilyIndices indices = findQueueFamilies();

		vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
	}
	void JeDevice::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies();

		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueInfos;

		if (indices.graphicsFamily == indices.presentFamily)
		{
			VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
			graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			graphicsQueueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
			graphicsQueueCreateInfo.queueCount = 1;
			graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
			queueInfos.push_back(graphicsQueueCreateInfo);
		}
		else
		{
			VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
			graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			graphicsQueueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
			graphicsQueueCreateInfo.queueCount = 1;
			graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
			queueInfos.push_back(graphicsQueueCreateInfo);

			VkDeviceQueueCreateInfo presentQueueCreateInfo{};
			presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			presentQueueCreateInfo.queueFamilyIndex = indices.presentFamily;
			presentQueueCreateInfo.queueCount = 1;
			presentQueueCreateInfo.pQueuePriorities = &queuePriority;
			queueInfos.push_back(presentQueueCreateInfo);
		}

		VkPhysicalDeviceFeatures physicalDeviceFeatures{};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

		if (validationLayersEnabled)
		{
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}

		std::vector<const char*> deviceExtensions;
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		deviceCreateInfo.enabledExtensionCount = 1;
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");
	}
	void JeDevice::logSurfaceInfo()
	{
		if (surface != VK_NULL_HANDLE)
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

			std::vector<VkSurfaceFormatKHR> formats(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

			SDL_Log("Surface supports %u formats:", formatCount);
			for (int i = 0; i < formatCount; i++)
			{
				SDL_Log("\tFormat %u: VkFormat=%d, ColorSpace=%d", i, formats[i].format, formats[i].colorSpace);
			}

			uint32_t modeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, nullptr);

			std::vector<VkPresentModeKHR> modes(modeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, modes.data());

			SDL_Log("Surface supports %u present modes:", modeCount);
			for (int i = 0; i < modeCount; i++)
			{
				SDL_Log("\tPresentMode %u: %d", i, modes[i]);
			}
		}
	}
	const char* JeDevice::getDeviceType(const int type) const
	{
		switch (type)
		{
		case 0:
			return "OTHER";
			break;
		case 1:
			return "INTEGRATED GPU";
			break;
		case 2:
			return "DISCRETE GPU";
			break;
		case 3:
			return "VIRTUAL GPU";
			break;
		case 4:
			return "CPU";
			break;
		default:
			return "UNKNOWN";
			break;
		}
	}
	
}