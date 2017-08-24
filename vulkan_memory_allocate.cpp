#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <vulkan/vulkan.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <time.h>

#include "common/utils.h"
#include "common/vk_logical_device.h"
#include "common/vk_memory_allocation.h"

typedef struct allocation_record_s
{
	void*			location;
	size_t			size;
	unsigned int	aquisition_time;
	unsigned int	release_time;
} allocation_record_t;

typedef std::map<void*, allocation_record_t> allocation_record_list_t;

typedef struct allocation_counter_s
{
	allocation_record_list_t	locations;
} allocation_counter_t;

void* memfn_alloc (void* user_data,
				   size_t size,
				   size_t alignment,
				   VkSystemAllocationScope scope)
{
	auto cntr = reinterpret_cast<allocation_counter_t*>(user_data);
	auto mem = malloc(size);

	std::cout << "alloc " << size << std::endl;

	if (mem != NULL)
	{
		cntr->locations.insert(std::make_pair (mem, allocation_record_t { mem, size, clock(), 0 }));
	}
	
	return mem;
}

void* memfn_realloc(void* user_data,
					void* original,
					size_t size,
					size_t alignment,
					VkSystemAllocationScope scope)
{
	auto	cntr = reinterpret_cast<allocation_counter_t*>(user_data);

	auto loc = cntr->locations.find(original);

	if (loc != cntr->locations.end())
	{
		loc->second.size = size;
		loc->second.aquisition_time = clock();
	}

	return realloc (original, size);
}

void memfn_free(void* user_data,
				void* location)
{
	auto cntr = reinterpret_cast<allocation_counter_t*>(user_data);
	auto loc = cntr->locations.find(location);

	if (loc != cntr->locations.end())
	{
		loc->second.release_time = clock();
	}

	free(location);
}

void memfn_internal_alloc_notification(void* user_data,
									   size_t size,
									   VkInternalAllocationType type,
									   VkSystemAllocationScope scope)
{
	auto cntr = reinterpret_cast<allocation_counter_t*>(user_data);

	std::cout << "memfn_internal_allocation" << std::endl;
}

void memfn_internal_free_notification(void* user_data,
									  size_t size,
									  VkInternalAllocationType type,
									  VkSystemAllocationScope scope)
{
	auto cntr = reinterpret_cast<allocation_counter_t*>(user_data);

	std::cout << "memfn_internal_free" << std::endl;
}

int main(int argc, char **argv)
{
	VkInstance				inst;
	VkInstanceCreateInfo	inst_info;

	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.enabledExtensionCount = 0;
	inst_info.enabledLayerCount = 0;
	inst_info.flags = 0;
	inst_info.pApplicationInfo = NULL;
	inst_info.pNext = NULL;
	inst_info.ppEnabledExtensionNames = NULL;
	inst_info.ppEnabledLayerNames = NULL;

	allocation_counter_t	instance_allocation_counter;

	VkAllocationCallbacks	instance_allocator_callbacks =
	{
		(void*)&instance_allocation_counter,
		memfn_alloc,
		memfn_realloc,
		memfn_free,
		memfn_internal_alloc_notification,
		memfn_internal_free_notification
	};

	switch (vkCreateInstance(&inst_info, &instance_allocator_callbacks, &inst))
	{
		case VK_SUCCESS:
		{
			break;
		}
		case VK_ERROR_INCOMPATIBLE_DRIVER:
		{
			std::cout << "cannot find a compatible Vulkan ICD\n";
			break;
		}
		default:
		{
			std::cout << "unknown error\n";
		}
	}

	auto physical_devices = find_matching_devices(inst, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, 0, 0);
	auto queue_families = queue_families_with_any_properties(physical_devices.front (), VK_QUEUE_COMPUTE_BIT);
	auto logical_device = vk_logical_device::create_logical_device(inst, physical_devices.front (), queue_families.front (), 1, NULL);
	auto memory_index = find_heap_index_for_memory_type(physical_devices.front (), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).front();

	{
		vk_memory_allocation buffer_1(
			logical_device,
			1024 * 4,
			memory_index);

		vk_memory_allocation buffer_2(
			logical_device,
			1024 * 1024 * 4,
			memory_index);

		// memory is freed by raii
	}

	// display information about the memory
	// output is <location> <size> <lifespan>
	for (auto mem : instance_allocation_counter.locations)
	{
		std::cout << mem.first << " ";
		std::cout << mem.second.size << " ";
		std::cout << mem.second.release_time - mem.second.aquisition_time << std::endl;
	}

	// total the amounts
	auto host_size = std::accumulate(
		std::begin(instance_allocation_counter.locations),
		std::end(instance_allocation_counter.locations),
		0,
		[](size_t cnt, allocation_record_list_t::value_type mem)
		{
			return cnt += mem.second.size;
		});

	std::cout << host_size << " bytes allocated on host" << std::endl;

	// check for unreleased memory
	auto leak_cnt = std::count_if(
		std::begin(instance_allocation_counter.locations),
		std::end(instance_allocation_counter.locations),
		[](allocation_record_list_t::value_type mem)
		{
			return (mem.second.release_time == 0);
		});

	auto leak_size = std::accumulate(
		std::begin(instance_allocation_counter.locations),
		std::end(instance_allocation_counter.locations),
		0,
		[](size_t cnt, allocation_record_list_t::value_type mem)
		{
			return (mem.second.release_time == 0) ? cnt + mem.second.size : cnt;
		});

	std::cout << leak_size << " bytes unreleased in " << leak_cnt << " locations" << std::endl;

	return 0;
}