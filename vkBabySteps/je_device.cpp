#include "je_device.hpp"

#include <stdexcept>
#include <vector>
#include <SDL3/SDL_log.h>

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

			if (physicalDevice == nullptr)
			{
				SDL_Log("Failed to find discrete GPU, falling back to integrated");
				physicalDevice = integratedGPU;
			}
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
				VkSurfaceCapabilitiesKHR surfaceCapabilities{};
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

				SDL_Log("Surface max image count: %u", surfaceCapabilities.maxImageCount);
				SDL_Log("Surface current extent: %u x %u", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
				indices.presentFamily = i;
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
	}
	void JeDevice::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies();

		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures physicalDeviceFeatures{};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

		std::vector<const char*> deviceExtensions;
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		deviceCreateInfo.enabledExtensionCount = 1;
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");
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
			break;
		}
	}
}