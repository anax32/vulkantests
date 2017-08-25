#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <functional>

#include "common/utils.h"

#include "common/vk_result.h"
#include "common/vk_extensions.h"
#include "common/vk_instance.h"
#include "common/vk_logical_device.h"
#include "common/vk_alloc_buffer.h"
#include "common/vk_bind_buffer_memory.h"

int main(int argc, char **argv)
{
	vk_instance	inst;

	auto gpus = find_matching_devices(inst,
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
		0,
		0);

	auto compute_queue = queue_families_with_any_properties(gpus.front(), VK_QUEUE_COMPUTE_BIT);

	auto memory_index = find_heap_index_for_memory_type(gpus.front(),
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT).front();

	vk_logical_device	device (
		inst,
		gpus.front(),
		compute_queue.front());

	// create the compute queue
	VkQueue	queue;
	vkGetDeviceQueue(device, compute_queue.front(), 0, &queue);

	{
		auto sz = 1024;
		auto res = VK_SUCCESS;

		// allocate a host buffer and do a memmap
		vk_alloc_buffer	host_buffer(
			device,
			sz,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			memory_index);

		void *ptr = nullptr;

		res = vkMapMemory(
			device,
			host_buffer,
			0,
			256,
			0,
			(void**)&ptr);

		auto fptr = (float*)ptr;
		for (auto i = 0; i < sz; i++)
		{
			*fptr++ = 1.0f;
		}

		vkUnmapMemory(
			device,
			host_buffer);

		// allocate a device buffer and upload some data
		vk_alloc_buffer	device_buffer(
			device,
			sz,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			memory_index);

		// raii cleanup
	}

	// raii cleanup
	return 0;
}
