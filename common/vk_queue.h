#include "vk_result.h"

#ifndef VK_QUEUE_H
#define VK_QUEUE_H
class vk_queue : public vk_result
{
protected:
	VkQueue	queue_;
public:
	vk_queue(VkDevice device, uint32_t family, uint32_t index)
		: queue_(VK_NULL_HANDLE)
	{
		vkGetDeviceQueue(device, family, index, &queue_);
	}
	vk_queue(VkDevice device, uint32_t family)
		: vk_queue(device, family, 0)
	{}
	operator VkQueue () const { return queue_; }
	void wait()
	{
		vkQueueWaitIdle(*this);
	}
};
#endif