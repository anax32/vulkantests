#include "vk_result.h"
#include "vk_instance.h"

#ifndef VK_LOGICAL_DEVICE_H
#define VK_LOGICAL_DEVICE_H
class vk_logical_device : public vk_result
{
public:
	static VkDevice create_logical_device(VkInstance instance,
										  VkPhysicalDevice device,
										  int queue_family,
										  int queue_count,
										  VkAllocationCallbacks* device_allocator_callbacks)
	{
		VkPhysicalDeviceProperties query_pdp = { 0 };
		query_pdp.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

		std::vector<float>	queue_priorites;
		queue_priorites.resize(queue_count);
		std::fill(
			std::begin(queue_priorites),
			std::end(queue_priorites),
			0.5f);

		VkDeviceQueueCreateInfo queue_info;
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.pNext = NULL;
		queue_info.flags = 0;
		queue_info.queueFamilyIndex = queue_family;
		queue_info.queueCount = queue_priorites.size();
		queue_info.pQueuePriorities = queue_priorites.data();

		// create a logical device
		VkDeviceCreateInfo	create_info;
		VkDevice			logical_device = nullptr;

		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pNext = NULL;
		create_info.flags = 0;
		create_info.queueCreateInfoCount = 1;	// probably important
		create_info.pQueueCreateInfos = &queue_info;
		create_info.enabledLayerCount = 0;		// deprecated
		create_info.ppEnabledExtensionNames = NULL;	// deprecated;
		create_info.enabledExtensionCount = 0;
		create_info.ppEnabledExtensionNames = NULL;
		create_info.pEnabledFeatures = NULL;

		vkCreateDevice(
			device,
			&create_info,
			device_allocator_callbacks,
			&logical_device);

		return logical_device;
	}
protected:
	VkInstance	instance_;
	VkDevice	device_;
public:
	vk_logical_device(VkInstance instance, VkPhysicalDevice physical_device, uint32_t queue_family)
		: instance_(instance)
	{
		device_ = create_logical_device(instance_,
			physical_device,
			queue_family,
			1,
			NULL);
	}
	virtual ~vk_logical_device()
	{
		vkDestroyDevice(device_, NULL);
	}
	operator VkDevice () const { return device_; }
};
#endif