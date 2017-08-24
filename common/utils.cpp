#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

#ifndef UTILS_STL_EXTNS_H
#define UTILS_STL_EXTNS_H
template<typename T>
size_t count_not_equal(T* start, size_t length, T value)
{
	return std::count_if(
		start,
		start + length,
		[value](const T& x)
	{return (x != value); });
}

template<typename T>
bool are_equal(T* first1, T* second1, size_t length)
{
	return std::equal(first1,
		first1 + length,
		second1);
}

template<typename T>
bool all_flags(T a, T b)
{
	return ((a & b) == a);
}

template<typename T>
bool any_flags(T a, T b)
{
	return ((a & b) != 0);
}
#endif

bool compare(VkPhysicalDeviceProperties& a, VkPhysicalDeviceProperties& b)
{
	if ((a.apiVersion != 0) && (b.apiVersion != 0) && (a.apiVersion != b.apiVersion))
		return false;

	if ((a.driverVersion != 0) && (b.driverVersion != 0) && (a.driverVersion != b.driverVersion))
		return false;

	if ((a.vendorID != 0) && (b.vendorID != 0) && (a.vendorID != b.vendorID))
		return false;

	if ((a.deviceID != 0) && (b.deviceID != 0) && (a.deviceID != b.deviceID))
		return false;

	if ((a.deviceType != 0) && (b.deviceType != 0) && (a.deviceType != b.deviceType))
		return false;

	auto a_name_len = count_not_equal(a.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, '\0');
	auto b_name_len = count_not_equal(b.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, '\0');

	if ((a_name_len != 0) && (b_name_len != 0) &&
		(
		(a_name_len != b_name_len) ||
		((a_name_len == b_name_len) &&
		(are_equal(a.deviceName, b.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE) == false)
		)
		))
		return false;

	auto a_uuid_len = count_not_equal(a.pipelineCacheUUID, VK_UUID_SIZE, (uint8_t)0);
	auto b_uuid_len = count_not_equal(b.pipelineCacheUUID, VK_UUID_SIZE, (uint8_t)0);

	if ((a_uuid_len != 0) && (b_uuid_len != 0) &&
		(
		(a_name_len != b_name_len) ||
		((a_name_len == b_name_len) &&
		(are_equal(a.pipelineCacheUUID, b.pipelineCacheUUID, VK_UUID_SIZE) == false)
		)
		))
		return false;

	// FIXME: check the limits and spare properties

	return true;
}

int find_queue_index_for_all_family_properties (VkPhysicalDevice dev, VkQueueFlags queue_properties)
{
	// check the queue flags
	uint32_t								queue_cnt = 0;
	std::vector<VkQueueFamilyProperties>	queue_props;

	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_cnt, NULL);
	queue_props.resize(queue_cnt);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_cnt, queue_props.data());

	for (auto i = 0; i < queue_props.size(); i++)
	{
		if (all_flags(queue_props[i].queueFlags, queue_properties))
		{
			return i;
		}
	}

	return -1;
}

std::vector<int> queue_families_with_properties(VkPhysicalDevice dev,
											    VkQueueFlags queue_properties,
												std::function<bool(VkQueueFlags, VkQueueFlags)> flag_comparision)
{
	std::vector<int>	retval;

	// check the queue flags
	uint32_t								queue_cnt = 0;
	std::vector<VkQueueFamilyProperties>	queue_props;

	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_cnt, NULL);
	queue_props.resize(queue_cnt);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_cnt, queue_props.data());

	for (auto i = 0; i < queue_props.size(); i++)
	{
		if (any_flags(queue_props[i].queueFlags, queue_properties))
		{
			retval.push_back (i);
		}
	}

	return retval;
}

std::vector<int> queue_families_with_all_properties(VkPhysicalDevice dev, VkQueueFlags queue_properties)
{
	return queue_families_with_properties(dev, queue_properties, all_flags<VkQueueFlags>);
}

std::vector<int> queue_families_with_any_properties(VkPhysicalDevice dev, VkQueueFlags queue_properties)
{
	return queue_families_with_properties(dev, queue_properties, any_flags<VkQueueFlags>);
}

std::vector<int> find_heap_index_for_memory_type(VkPhysicalDevice physical_device, VkMemoryPropertyFlags memory_properties)
{
	VkPhysicalDeviceMemoryProperties	dev_mem;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &dev_mem);
	std::vector<int>	heaps;

	for (auto i = 0; i < dev_mem.memoryTypeCount; i++)
	{
		auto& type = dev_mem.memoryTypes[i];

		if (any_flags(type.propertyFlags, memory_properties))
		{
			heaps.push_back(i);
		}
	}

	return heaps;
}

std::vector<VkPhysicalDevice> find_matching_devices (VkInstance instance,
												     VkPhysicalDeviceProperties physical_device_properties,
												     VkMemoryPropertyFlags memory_properties,
												     VkQueueFlags queue_properties)
{
	std::vector<VkPhysicalDevice>	retval;
	std::vector<VkPhysicalDevice>	device_list;
	uint32_t						device_cnt = 0;
	vkEnumeratePhysicalDevices(instance, &device_cnt, NULL);
	device_list.resize(device_cnt);
	vkEnumeratePhysicalDevices(instance, &device_cnt, device_list.data());

	for (auto& dev : device_list)
	{
		// check the device properties
		VkPhysicalDeviceProperties			dev_prop;
		vkGetPhysicalDeviceProperties(dev, &dev_prop);

		if (compare(physical_device_properties, dev_prop) == false)
			continue;

		// check the memory type flags
		VkPhysicalDeviceMemoryProperties	dev_mem;
		vkGetPhysicalDeviceMemoryProperties(dev, &dev_mem);

		if (std::any_of(dev_mem.memoryTypes,
			dev_mem.memoryTypes + dev_mem.memoryTypeCount,
			[memory_properties](const VkMemoryType& type)
			{
				return all_flags (type.propertyFlags, memory_properties);
			}) == false)
		{
			continue;
		}

		// check the queue flags
		if ((queue_properties != 0) &&
			(queue_families_with_all_properties(dev, queue_properties).empty() == true))
		{
			continue;
		}

		retval.push_back(dev);
	}

	return retval;
}


std::vector<VkPhysicalDevice> find_matching_devices(VkInstance instance,
													VkPhysicalDeviceType physical_device_type,
													VkMemoryPropertyFlags memory_properties,
													VkQueueFlags queue_properties)
{
	VkPhysicalDeviceProperties query_pdp = { 0 };
	query_pdp.deviceType = physical_device_type;

	return find_matching_devices(instance,
								 query_pdp,
								 memory_properties,
								 queue_properties);
}