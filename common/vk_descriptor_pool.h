#include "vk_result.h"

#ifndef VK_DESCRIPTOR_POOL_H
#define VK_DESCRIPTOR_POOL_H
class vk_descriptor_pool : public vk_result
{
protected:
	VkDevice			device_;
	VkDescriptorPool	pool_;
public:
	vk_descriptor_pool(VkDevice device)
		: device_(device)
	{
		VkDescriptorPoolSize	pool_size =
		{
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			2
		};

		VkDescriptorPoolCreateInfo create_info = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			NULL,
			0,
			1,
			1,
			&pool_size
		};

		result = vkCreateDescriptorPool(
			device_,
			&create_info,
			NULL,
			&pool_);
	}
	virtual ~vk_descriptor_pool()
	{
		vkDestroyDescriptorPool(
			device_,
			pool_,
			NULL);
	}
	operator VkDescriptorPool () const { return pool_; }
};
#endif