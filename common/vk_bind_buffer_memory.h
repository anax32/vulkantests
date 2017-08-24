#ifndef VK_BIND_BUFFER_MEMORY_H
#define VK_BIND_BUFFER_MEMORY_H
class vk_bind_buffer_memory : public vk_result
{
protected:
	VkBuffer		buffer_;
	VkDeviceMemory	device_mem_;
public:
	vk_bind_buffer_memory(VkDevice device,
						  VkBuffer memory_buffer,
						  VkDeviceMemory device_memory)
		: buffer_(memory_buffer),
		  device_mem_(device_memory)
	{
		result = vkBindBufferMemory(
			device,
			buffer_,
			device_mem_,
			0);
	}
};
#endif