#include "vk_memory_allocation.h"
#include "vk_buffer.h"
#include "vk_bind_buffer_memory.h"

#ifndef VK_ALLOC_BUFFER_H
#define VK_ALLOC_BUFFER_H
class vk_alloc_buffer
{
	// member constructor order is declaration order
protected:
	vk_buffer				buffer_;
	vk_buffer_requirements	req_;
public:
	vk_memory_allocation	memory;
protected:
	vk_bind_buffer_memory	bind_;
public:
	vk_alloc_buffer(VkDevice device,
					size_t size,
					VkBufferUsageFlags usage,
					VkSharingMode sharing_mode,
					uint32_t memory_type_index)
		: buffer_(device, size, usage, sharing_mode),
		  req_(device, buffer_),
		  memory (device, req_, memory_type_index),
		  bind_(device, buffer_, memory)
	{}

	operator VkDeviceMemory () const { return static_cast<VkDeviceMemory>(memory); }
	operator VkBuffer () const { return static_cast<VkBuffer>(buffer_); }
};
#endif