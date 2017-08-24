class vk_descriptor_set_layout
{
	// https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VUID-VkDescriptorSetLayoutBinding-descriptorType-parameter
protected:
	VkDevice				device_;
	VkDescriptorSetLayout	layout_;

public:
	vk_descriptor_set_layout(VkDevice device, std::vector<VkDescriptorSetLayoutBinding> bindings)
		: device_(device)
	{
		VkDescriptorSetLayoutCreateInfo create_info =
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			NULL,
			0,
			static_cast<uint32_t>(bindings.size()),
			bindings.data()
		};

		auto result = vkCreateDescriptorSetLayout(
			device_,
			&create_info,
			NULL,
			&layout_);
	}
	virtual ~vk_descriptor_set_layout()
	{
		vkDestroyDescriptorSetLayout(device_, layout_, NULL);
	}

	operator VkDevice () const	{ return device_; }
	operator VkDescriptorSetLayout () const { return layout_; }
	operator const VkDescriptorSetLayout* () const { return &layout_; }

	uint32_t size() const { return 1; }
};
