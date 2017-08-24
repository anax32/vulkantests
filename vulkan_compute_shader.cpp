#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
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
#include "common/vk_physical_device.h"
#include "common/vk_logical_device.h"
#include "common/vk_queue.h"
#include "common/vk_descriptor_pool.h"
#include "common/vk_descriptor_set_layout.h"
#include "common/vk_descriptor_set.h"
#include "common/vk_shader_module.h"
#include "common/vk_alloc_buffer.h"
#include "common/vk_command_pool.h"
#include "common/vk_command_buffer.h"
#include "common/vk_pipeline_layout.h"

class vk_compute_pipeline : public vk_result
{
protected:
	VkDevice	device_;
	VkPipeline	pipeline_;

public:
	vk_compute_pipeline(VkDevice device,
						VkShaderModule shader_module,
						const char *entry_point,
						VkPipelineLayout pipeline_layout)
		: device_(device)
	{
		VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info =
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			NULL,
			0,
			VK_SHADER_STAGE_COMPUTE_BIT,
			shader_module,
			entry_point,
			NULL
		};

		// create the pipeline
		VkComputePipelineCreateInfo pipeline_create_info =
		{
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			NULL,
			0,
			pipeline_shader_stage_create_info,
			pipeline_layout,
			VK_NULL_HANDLE,
			-1
		};

		result = vkCreateComputePipelines(
			device_,
			VK_NULL_HANDLE,
			1,
			&pipeline_create_info,
			NULL,
			&pipeline_);
	}
	virtual ~vk_compute_pipeline()
	{
		vkDestroyPipeline(device_, pipeline_, NULL);
	}
	operator VkPipeline () const { return pipeline_; }
};

int main(int argc, char **argv)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	vk_instance	inst({}, { VK_EXT_DEBUG_REPORT_EXTENSION_NAME });

	auto gpus = find_matching_devices(inst,
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
		0,
		0);

	if (gpus.empty() == true)
	{
		std::cout << "ERR: No compute devices" << std::endl;
		return -1;
	}

	vk_physical_device physical_device(gpus.front());

	auto compute_queue = physical_device.get_compute_family();
	auto device_memory_heaps = physical_device.get_device_heaps();
	auto host_memory_heaps = physical_device.get_host_heaps();

	if (device_memory_heaps.empty() == true)
	{
		std::cout << "ERR: No device heaps" << std::endl;
		return -1;
	}

	if (host_memory_heaps.empty() == true)
	{
		std::cout << "ERR: No host heaps" << std::endl;
		return -1;
	}

	auto device_memory_index = device_memory_heaps.front();
	auto host_memory_index = host_memory_heaps.front();

	const unsigned int data_length = 1024;
	std::vector<float>	send(data_length);
	std::vector<float>	recv(data_length);
	unsigned int		i = 0;
	std::generate(std::begin(send), std::end(send), [&i](){return static_cast<float>(++i); });

	// create the compute queue
	vk_logical_device			device(
		inst,
		physical_device,
		compute_queue.front());

	vk_queue					queue(
		device,
		compute_queue.front());

	// allocate the buffer
	vk_alloc_buffer				device_memory(
		device,
		1024,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		device_memory_index);

	vk_alloc_buffer				host_memory(
		device,
		1024,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		host_memory_index);

	// create the shader
	vk_shader_module			shader_module(
		device,
		vk_shader_module::read_spir_v_file("shader.comp.spv"));

	vk_descriptor_pool			descriptor_pool(
		device);

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
			},
			{
				1,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				1,
				VK_SHADER_STAGE_COMPUTE_BIT,
				NULL
			}
		});

	vk_descriptor_set			descriptor_set(
		device,
		descriptor_pool,
		descriptor_set_layout);

	descriptor_set.bind(
	{
		vk_descriptor_buffer_info(device_memory),
		vk_descriptor_buffer_info(device_memory)
	},
	{
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	},
	{
		0,
		1
	});

	// create the pipeline layout
	vk_pipeline_layout			pipeline_layout(
		device,
		{ descriptor_set_layout },
		{});

	// create the pipeline
	vk_compute_pipeline			pipeline(
		device,
		shader_module,
		"main",
		pipeline_layout);


	// create the command pool
	vk_command_pool	command_pool(device, compute_queue.front());

	// create the command buffer
	VkBufferCopy fixed_memory_region{
		0, 0, sizeof(float) * send.size()
	};
	vk_command_buffer		cmd_copy_memory_over(device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
	vk_command_buffer		cmd_copy_memory_back(device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
	vk_command_buffer		cmd_apply_shader(device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	// record the memory copy commands
	cmd_copy_memory_over.begin();
	vkCmdCopyBuffer(
		cmd_copy_memory_over,
		host_memory,
		device_memory,
		1, &fixed_memory_region);
	cmd_copy_memory_over.end();

	cmd_copy_memory_back.begin();
	vkCmdCopyBuffer(
		cmd_copy_memory_back,
		device_memory,
		host_memory,
		1, &fixed_memory_region);
	cmd_copy_memory_back.end();

	// record the command buffer
	cmd_apply_shader.begin();
	cmd_apply_shader.bind(pipeline);
	cmd_apply_shader.bind(pipeline_layout, descriptor_set);
	cmd_apply_shader.dispatch(1024, 1, 1);
	cmd_apply_shader.end();

	// upload the data
	host_memory.memory.put(send.data(), send.size());

	// execute the commands
	cmd_copy_memory_over.submit(queue);
	vkQueueWaitIdle(queue);

	cmd_apply_shader.submit(queue);
	vkQueueWaitIdle(queue);

	cmd_copy_memory_back.submit(queue);
	vkQueueWaitIdle(queue);

	// read the data
	host_memory.memory.get(recv.data(), recv.size());

	return 0;
}