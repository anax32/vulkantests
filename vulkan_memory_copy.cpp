#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>

#include "common/utils.h"

#include "common/vk_result.h"
#include "common/vk_extensions.h"
#include "common/vk_instance.h"
#include "common/vk_logical_device.h"
#include "common/vk_alloc_buffer.h"
#include "common/vk_command_pool.h"
#include "common/vk_command_buffer.h"

int main(int argc, char **argv)
{
	vk_instance	inst;

	const unsigned int	data_length = 1024;
	std::vector<float>	send (data_length);
	std::vector<float>	recv (data_length);
	auto i = 0;
	std::generate(std::begin(send), std::end(send), [i]()mutable { return i++; });

	{
		auto gpus = find_matching_devices(inst,
			VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
			0,
			0);

		auto compute_queue = queue_families_with_any_properties(gpus.front(), VK_QUEUE_COMPUTE_BIT);

		auto device_memory_index = find_heap_index_for_memory_type(
			gpus.front(),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).front();

		auto host_memory_index = find_heap_index_for_memory_type(
			gpus.front(),
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT).front();

		// create the compute queue
		vk_logical_device			device(
			inst,
			gpus.front(),
			compute_queue.front());

		VkQueue	queue;
		vkGetDeviceQueue(device, compute_queue.front(), 0, &queue);

		// allocate the buffers
		vk_alloc_buffer				host_memory_0(
			device,
			data_length*sizeof(float),
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			host_memory_index);

		vk_alloc_buffer				host_memory_1(
			device,
			data_length*sizeof(float),
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			host_memory_index);

		vk_alloc_buffer				device_memory_0(
			device,
			data_length*sizeof(float),
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			device_memory_index);

		// create the command pool
		vk_command_pool				command_pool(
			device,
			compute_queue.front());

		// create the command buffer
		vk_command_buffer			copy_host_to_host_command(
			device,
			command_pool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1);

		vk_command_buffer			clear_host_1_command(
			device,
			command_pool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1);

		vk_command_buffer			copy_host0_device_host1_command(
			device,
			command_pool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1);

		// record the command buffer for host to host copy
		copy_host_to_host_command.begin();
		VkBufferCopy fixed_memory_region{
			0, 0, sizeof(float) * send.size()
		};
		vkCmdCopyBuffer(
			copy_host_to_host_command,
			host_memory_0,
			host_memory_1,
			1, &fixed_memory_region);
		copy_host_to_host_command.end();

		clear_host_1_command.begin();
		vkCmdFillBuffer(
			clear_host_1_command,
			host_memory_1,
			0, VK_WHOLE_SIZE,
			0);
		clear_host_1_command.end();

		copy_host0_device_host1_command.begin();
		vkCmdCopyBuffer(
			copy_host0_device_host1_command,
			host_memory_0,
			device_memory_0,
			1, &fixed_memory_region);
		vkCmdCopyBuffer(
			copy_host0_device_host1_command,
			device_memory_0,
			host_memory_1,
			1, &fixed_memory_region);
		copy_host0_device_host1_command.end();

		// memory map the first host buffer
		host_memory_0.memory.put(send.data(), send.size());

		// execute the host to host copy
		copy_host_to_host_command.submit(queue);
		vkQueueWaitIdle(queue);

		// check the result
		host_memory_1.memory.get(recv.data (), recv.size ());
		auto host_to_host_copy = std::equal(std::begin(send),
											std::end(send),
											std::begin(recv));

		// clear the host 1 buffer
		host_memory_1.memory.get(recv.data(), recv.size());
		clear_host_1_command.submit(queue);
		vkQueueWaitIdle(queue);
		// check the result
		host_memory_1.memory.get(recv.data(), recv.size());
		auto non_zero = std::count_if(std::begin(recv),
									  std::end(recv),
									  [](float x) {return std::abs(x) > std::numeric_limits<float>::min(); });

		// execute the host0 to device to host1 copy
		host_memory_0.memory.put(send.data(), send.size());
		copy_host0_device_host1_command.submit(queue);
		vkQueueWaitIdle(queue);

		// check the result
		host_memory_1.memory.get(recv.data(), recv.size());
		auto host_to_device_to_host_copy = std::equal(std::begin(send),
													  std::end(send),
													  std::begin(recv));

		if (host_to_host_copy == false)
		{
			std::cout << "ERR: host to host copy failed" << std::endl;
		}
		else
		{
			std::cout << "SUC: host to host copy worked" << std::endl;
		}

		if (non_zero > 0)
		{
			std::cout << "ERR: clear buffer failed to zero all elements" << std::endl;
		}
		else
		{
			std::cout << "SUC: clear buffer to zero on all elements worked" << std::endl;
		}

		if (host_to_device_to_host_copy == false)
		{
			std::cout << "ERR: host to device to host copy failed" << std::endl;
		}
		else
		{
			std::cout << "SUC: host to device to host copy worked" << std::endl;
		}


		// raii cleanup
	}
	return 0;
}
