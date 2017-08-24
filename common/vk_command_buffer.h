#include "vk_result.h"

#ifndef VK_COMMAND_BUFFER_H
#define VK_COMMAND_BUFFER_H
// TODO: specialise this class into graphics_buffer and compute_buffer subclasses,
// which can chose their own VkQueues and pipelines and dispatch/render functions
class vk_command_buffer : public vk_result
{
protected:
	VkDevice device_;
	VkCommandPool command_pool_;
	VkCommandBuffer	buffer_;
public:
	vk_command_buffer(VkDevice device, VkCommandPool command_pool, VkCommandBufferLevel level, uint32_t count)
		: device_(device),
		command_pool_(command_pool)
	{
		VkCommandBufferAllocateInfo	alloc_info =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			NULL,
			command_pool_,
			level,	// VK_COMMAND_BUFFER_LEVEL_PRIMARY
			count
		};

		//vkAllocateCommandBuffers(device_, &alloc_info, &buffers.data()[0]);
		result = vkAllocateCommandBuffers(device_, &alloc_info, &buffer_);
	}
	virtual ~vk_command_buffer()
	{
		vkFreeCommandBuffers(
			device_,
			command_pool_,
			1,
			&buffer_);
	}
	operator VkCommandBuffer () const { return buffer_; }
	void begin()
	{
		VkCommandBufferBeginInfo	begin =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			NULL,
			0,
			NULL
		};

		result = vkBeginCommandBuffer(
			buffer_,
			&begin);
	}
	void end()
	{
		result = vkEndCommandBuffer(buffer_);
	}
	void bind (VkPipeline pipeline)
	{
		vkCmdBindPipeline(
			buffer_,
			VK_PIPELINE_BIND_POINT_COMPUTE,
			pipeline);
	}
	void bind (VkPipelineLayout pipeline_layout, VkDescriptorSet descriptor_set)
	{
		vkCmdBindDescriptorSets(
			buffer_,
			VK_PIPELINE_BIND_POINT_COMPUTE,
			pipeline_layout,
			0,
			1,
			&descriptor_set,
			0,
			NULL);
	}
	void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
	{
		vkCmdDispatch(buffer_, group_count_x, group_count_y, group_count_z);
	}
	void submit(VkQueue queue)
	{
		VkPipelineStageFlags flags[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

		VkSubmitInfo	submit_info = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			NULL,
			0, NULL,	// wait semaphores
			flags,
			1, &buffer_,	// command buffers
			0, NULL		// signal semaphores
		};

		result = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
	}
};
#endif