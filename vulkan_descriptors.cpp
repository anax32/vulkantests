#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <functional>

#include "common/utils.h"

#include "common/vk_extensions.h"
#include "common/vk_instance.h"
#include "common/vk_logical_device.h"
#include "common/vk_descriptor_set_layout.h"
#include "common/vk_descriptor_pool.h"
#include "common/vk_descriptor_set.h"

int main (int argc, char **argv)
{
	vk_instance	inst;

	auto gpus = find_matching_devices (inst,
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
		0,
		0);

	auto compute_queue = queue_families_with_any_properties (gpus.front (), VK_QUEUE_COMPUTE_BIT);

	vk_logical_device			device (
		inst,
		gpus.front(),
		compute_queue.front());

	// set the memory layouts
	vk_descriptor_set_layout	descriptor_set_layout(
		device,
		{
			{
				0,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				NULL
			}
		});

	vk_descriptor_pool			descriptor_pool(
		device);

	vk_descriptor_set			descriptor_set(
		device,
		descriptor_pool,
		descriptor_set_layout);

	// raii cleanup
	return 0;
}
