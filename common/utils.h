#include <functional>
#include <vector>

extern "C++"
int find_queue_index_for_all_family_properties(VkPhysicalDevice dev,
											   VkQueueFlags queue_properties);

//extern "C++"
//int find_queue_index_for_any_family_properties(VkPhysicalDevice dev,
//											   VkQueueFlags queue_properties);
extern "C++"
std::vector<int> queue_families_with_properties(VkPhysicalDevice dev, VkQueueFlags queue_properties, std::function<bool(VkQueueFlags, VkQueueFlags)>);

extern "C++"
std::vector<int> queue_families_with_any_properties(VkPhysicalDevice dev, VkQueueFlags queue_properties);

extern "C++"
std::vector<int> queue_families_with_all_properties(VkPhysicalDevice dev, VkQueueFlags queue_properties);

extern "C++"
std::vector<int> find_heap_index_for_memory_type(VkPhysicalDevice physical_device,
												 VkMemoryPropertyFlags memory_properties);

extern "C++"
std::vector<VkPhysicalDevice> find_matching_devices (VkInstance instance,
												     VkPhysicalDeviceProperties physical_device_properties,
												     VkMemoryPropertyFlags memory_properties,
												     VkQueueFlags queue_properties);

extern "C++"
std::vector<VkPhysicalDevice> find_matching_devices(VkInstance instance,
													VkPhysicalDeviceType physical_device_type,
													VkMemoryPropertyFlags memory_properties,
													VkQueueFlags queue_properties);

extern "C++"
VkDevice create_logical_device(VkInstance instance,
							   VkPhysicalDevice device,
							   int queue_family,
							   int queue_count,
							   VkAllocationCallbacks* device_allocator_callbacks);