#include "vk_result.h"

#ifndef VK_COMMAND_POOL_H
#define VK_COMMAND_POOL_H
class vk_command_pool : public vk_result
{
protected:
	VkDevice device_;
	VkCommandPool pool_;
public:
	vk_command_pool(VkDevice device, uint32_t queue_family_index)
		: device_(device)
	{
		VkCommandPoolCreateInfo	create_info =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			NULL,
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			queue_family_index
		};

		result = vkCreateCommandPool(device_, &create_info, NULL, &pool_);
	}
	virtual ~vk_command_pool()
	{
		vkDestroyCommandPool(device_, pool_, NULL);
	}
	operator VkCommandPool () const { return pool_; }
};
#endif