#include "vk_result.h"

#ifndef VK_BUFFER_H
#define VK_BUFFER_H
class vk_buffer : vk_result
{
protected:
	VkDevice	device_;
	VkBuffer	buffer_;
public:
	vk_buffer(VkDevice device, size_t size, VkBufferUsageFlags usage, VkSharingMode sharing_mode)
		: device_(device)
	{
		VkBufferCreateInfo create_info = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			NULL,
			0,
			size,
			usage,
			sharing_mode,
			0,
			NULL };

		result = vkCreateBuffer(
			device_,
			&create_info,
			NULL,
			&buffer_);
	}
	virtual ~vk_buffer()
	{
		vkDestroyBuffer(device_, buffer_, NULL);
	}
	operator const VkBuffer () const { return buffer_; }
};
#endif

#ifndef VK_BUFFER_STORAGE_H
#define VK_BUFFER_STORAGE_H
class vk_buffer_storage : public vk_buffer
{
public:
	vk_buffer_storage(VkDevice device, size_t size)
		: vk_buffer(device, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE)
	{}
};
#endif

#ifndef VK_BUFFER_REQUIREMENTS_H
#define VK_BUFFER_REQUIREMENTS_H
class vk_buffer_requirements
{
protected:
	VkMemoryRequirements	req_;
public:
	vk_buffer_requirements(VkDevice device, VkBuffer buffer)
	{
		vkGetBufferMemoryRequirements(
			device,
			buffer,
			&req_);
	}
	operator VkMemoryRequirements () { return req_; }
	operator VkDeviceSize () { return req_.size; }
};
#endif