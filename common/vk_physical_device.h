#include "vk_result.h"
#include <vector>

#ifndef VK_PHYSICAL_DEVICE_H
#define VK_PHYSICAL_DEVICE_H
class vk_physical_device : public vk_result
{
protected:
	VkPhysicalDevice physical_device_;
public:
	vk_physical_device(VkPhysicalDevice physical_device)
		: physical_device_(physical_device)
	{}
	operator VkPhysicalDevice () const { return physical_device_; }
	// queue families
	auto get_queue_family(VkQueueFlagBits flags) -> std::vector<int>
	{
		return queue_families_with_any_properties(*this, flags);
	}
	auto get_compute_family() -> std::vector<int>
	{
		return get_queue_family(VK_QUEUE_COMPUTE_BIT);
	}
	// memory types
	auto get_memory_heaps(VkMemoryPropertyFlags flags) -> std::vector < int >
	{
		return find_heap_index_for_memory_type(physical_device_, flags);
	}
	auto get_device_heaps() -> std::vector < int >
	{
		return get_memory_heaps(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}
	auto get_host_heaps() -> std::vector < int >
	{
		return get_memory_heaps(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}
};
#endif