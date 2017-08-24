#ifndef vk_layers_h
#define vk_layers_h
class vk_layer_base
{
protected:
	VkInstance inst_;
public:
	vk_layer_base(VkInstance instance)
		: inst_(instance)
	{}
	virtual ~vk_layer_base()
	{}
	virtual operator const char*() = NULL;
};

class VK_LAYER_LUNARG_standard_validation_extension : public vk_layer_base
{
public:
	operator const char *()			{ return "VK_LAYER_LUNARG_standard_validation"; }
};
#endif