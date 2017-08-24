#include "vk_result.h"

#ifndef VK_PIPELINE_LAYOUT_H
#define VK_PIPELINE_LAYOUT_H
class vk_pipeline_layout : public vk_result
{
protected:
	VkDevice			device_;
	VkPipelineLayout	layout_;
public:
	vk_pipeline_layout(VkDevice device,
					   std::vector<VkDescriptorSetLayout> descriptor_set_layouts,
					   std::vector<VkPushConstantRange> push_constant_ranges)
		: device_(device)
	{
		VkPipelineLayoutCreateInfo pipeline_create_info =
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			NULL,
			0,
			static_cast<uint32_t>(descriptor_set_layouts.size()),
			descriptor_set_layouts.data(),
			static_cast<uint32_t>(push_constant_ranges.size()),
			push_constant_ranges.data(),
		};

		result = vkCreatePipelineLayout(
			device_,
			&pipeline_create_info,
			NULL,
			&layout_);
	}

	virtual ~vk_pipeline_layout()
	{
		vkDestroyPipelineLayout(device_, layout_, NULL);
	}
	operator VkPipelineLayout () const { return layout_; }
};
#endif
