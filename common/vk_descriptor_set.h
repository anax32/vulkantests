#include "vk_result.h"

#ifndef VK_DESCRIPTOR_SET_H
#define VK_DESCRIPTOR_SET_H

class vk_descriptor_buffer_info
{
protected:
	const VkDescriptorBufferInfo	info_;
public:
	vk_descriptor_buffer_info(VkBuffer buffer)
		: info_(VkDescriptorBufferInfo{ buffer, 0, VK_WHOLE_SIZE })
	{}
	vk_descriptor_buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
		: info_(VkDescriptorBufferInfo{ buffer, offset, range })
	{}
	operator VkDescriptorBufferInfo () const { return info_; }
};

class vk_descriptor_set : public vk_result
{
protected:
	VkDevice			device_;
	VkDescriptorPool	pool_;
	VkDescriptorSet		descriptor_set_;
public:
	vk_descriptor_set(VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout layout)
		: device_(device),
		  pool_(pool)
	{
		VkDescriptorSetAllocateInfo	alloc_info =
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			NULL,
			pool_,
			1,
			&layout
		};

		result = vkAllocateDescriptorSets(device_, &alloc_info, &descriptor_set_);
	}
	virtual ~vk_descriptor_set()
	{
		vkFreeDescriptorSets(device_, pool_, 1, &descriptor_set_);
	}
	operator VkDescriptorSet () const { return descriptor_set_; }

	void bind(std::vector<VkWriteDescriptorSet> update)
	{
		vkUpdateDescriptorSets(
			device_,
			update.size(),
			update.data(),
			0, NULL);
	}

	void bind(std::vector<VkDescriptorBufferInfo> buffer_info,
			  std::vector<VkDescriptorType> types,
			  std::vector<uint32_t> locations)
	{
		std::vector<VkWriteDescriptorSet>	writes;

		for (auto i = 0; i < buffer_info.size(); i++)
		{
			writes.push_back(
				VkWriteDescriptorSet
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					NULL,
					descriptor_set_,
					locations[i],
					0,
					1,
					types[i],
					NULL,
					&buffer_info[i],
					NULL
				});
		}

		bind(writes);
	}

	void bind(VkDescriptorBufferInfo buffer_info,
			  VkDescriptorType type,
			  uint32_t location)
	{
		bind(
		{ buffer_info },
		{ type },
		{ location });
	}
};
#endif