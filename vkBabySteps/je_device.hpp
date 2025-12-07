#pragma once
#include <vulkan/vulkan.h>

namespace je
{
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily = UINT32_MAX;
		uint32_t presentFamily = UINT32_MAX;
		bool isComplete() const { return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX; };
	};

	class JeDevice
	{
	public:
		JeDevice(const VkInstance& instance, VkSurfaceKHR surface);
		~JeDevice();

		JeDevice(const JeDevice&) = delete;
		JeDevice& operator=(const JeDevice&) = delete;

		VkDevice& get() { return device; };
	private:
		VkDevice device = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;

		VkSurfaceKHR surface = VK_NULL_HANDLE;

		void getPhysicalDevice(const VkInstance& instance);

		QueueFamilyIndices findQueueFamilies();
		void retrieveQueues();
		void createLogicalDevice();

		const char* getDeviceType(const int type) const;
	};
}