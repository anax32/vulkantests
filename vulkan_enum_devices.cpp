#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "common/utils.h"
#include "common/vk_instance.h"

void list_physical_device_properties (VkPhysicalDevice& dev)
{
	VkPhysicalDeviceProperties			dev_prop;

	// physical device properties
	vkGetPhysicalDeviceProperties (dev, &dev_prop);

	std::cout << "Device" << std::endl;
	std::cout << "------" << std::endl;
	std::cout << "  " << dev_prop.deviceName << std::endl;
	std::cout << "  ";

	switch (dev_prop.deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_OTHER" << std::endl;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU" << std::endl;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU" << std::endl;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU" << std::endl;
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_CPU" << std::endl;
		break;
	}
}

void list_physical_device_memory (VkPhysicalDevice& dev)
{
	VkPhysicalDeviceMemoryProperties	dev_mem;

	vkGetPhysicalDeviceMemoryProperties (dev, &dev_mem);

	std::cout << "  Heaps: " << dev_mem.memoryHeapCount << std::endl;

	for (auto i = 0; i < dev_mem.memoryHeapCount; i++)
	{
		std::cout << "    Size: ";
		std::cout << (dev_mem.memoryHeaps[i].size >> 20) << "Mb" << std::endl;

		for (auto j = 0; j < dev_mem.memoryTypeCount; j++)
		{
			if (dev_mem.memoryTypes[j].heapIndex != i)
				continue;

			if (dev_mem.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				std::cout << "      " << "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT" << std::endl;
			if (dev_mem.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				std::cout << "      " << "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT" << std::endl;
			if (dev_mem.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				std::cout << "      " << "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT" << std::endl;
			if (dev_mem.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
				std::cout << "      " << "VK_MEMORY_PROPERTY_HOST_CACHED_BIT" << std::endl;
			if (dev_mem.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
				std::cout << "      " << "VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT" << std::endl;
		}
	}
}

void list_physical_device_queue_families (VkPhysicalDevice& dev)
{
	// device queue properties
	uint32_t	queue_cnt = 0;
	std::vector<VkQueueFamilyProperties>	queue_props;

	vkGetPhysicalDeviceQueueFamilyProperties (dev, &queue_cnt, NULL);
	queue_props.resize (queue_cnt);
	vkGetPhysicalDeviceQueueFamilyProperties (dev, &queue_cnt, queue_props.data());

	std::cout << "  Queue Families: " << std::endl;

	for (auto& props : queue_props)
	{
		std::cout << "    Queues: " << props.queueCount << std::endl;
		std::cout << "    Stamps: " << ((props.timestampValidBits == 0) ? "no" : "yes") << std::endl;
		std::cout << "    Flags: " << std::endl;

		if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			std::cout << "      " << "VK_QUEUE_GRAPHICS_BIT" << std::endl;
		if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
			std::cout << "      " << "VK_QUEUE_COMPUTE_BIT" << std::endl;
		if (props.queueFlags & VK_QUEUE_TRANSFER_BIT)
			std::cout << "      " << "VK_QUEUE_TRANSFER_BIT" << std::endl;
		if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
			std::cout << "      " << "VK_QUEUE_SPARSE_BINDING_BIT" << std::endl;
	}
}

int main(int argc, char **argv)
{
	vk_instance				inst;

	// enum the devices
	auto device_list = find_matching_devices (inst, VkPhysicalDeviceProperties{ 0 }, 0, 0);

	for (auto& dev : device_list)
	{
		list_physical_device_properties (dev);
		list_physical_device_memory (dev);
		list_physical_device_queue_families (dev);
	}

	return 0;
}