#include <map>

#include "vk_result.h"

#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H
class vk_instance : public vk_result
{
protected:
	VkInstance	inst_;

#ifdef vk_layers_h
	std::map<std::string, vk_layer_base*>		layers;

	void append_layers(VkInstanceCreateInfo& info)
	{
		// FIXME: lol
	}
	void clean_layers()
	{
		for (auto k : layers)
		{
			delete k.second;
		}

		layers.clear();
	}
#endif
#ifdef vk_extension_h
	std::map<std::string, vk_extension_base*>	extensions;

	void append_extensions(VkInstanceCreateInfo& info)
	{
		for (auto i = 0; i < info.enabledExtensionCount; i++)
		{
			auto ext = std::string(info.ppEnabledExtensionNames[i]);

			if (std::equal(std::begin(ext), std::end(ext), VK_KHR_SURFACE_EXTENSION_NAME))
			{
				extensions.insert({ VK_KHR_SURFACE_EXTENSION_NAME, new VK_KHR_surface_extension(inst_) });
			}
			else if (std::equal(std::begin(ext), std::end(ext), VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
			{
				extensions.insert({ VK_EXT_DEBUG_REPORT_EXTENSION_NAME, new vk_debug_extension(inst_) });
			}
		}
	}
	void clean_extensions()
	{
		for (auto e : extensions)
		{
			delete e.second;
		}

		extensions.clear();
	}
#endif

public:
	vk_instance(VkInstanceCreateInfo& info)
	{
		result = vkCreateInstance(&info, NULL, &inst_);

#ifdef vk_layers_h
		append_layers(info);
#endif
#ifdef vk_extension_h
		append_extensions(info);
#endif
//		print_result();
	}
	vk_instance(std::vector<const char *> layers,
				std::vector<const char *> extensions)
		: vk_instance (VkInstanceCreateInfo
						{
							VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
							NULL,
							0,
							NULL,
							layers.size(),
							layers.data(),
							extensions.size(),
							extensions.data()
						})
	{}
	vk_instance()
		: vk_instance (VkInstanceCreateInfo
						{
							VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
						})
	{}
	virtual ~vk_instance()
	{
#ifdef vk_layers_h
		destroy_layers ();
#endif
#ifdef vk_extension_h
		clean_extensions();
#endif
		vkDestroyInstance(inst_, NULL);
	}
	// casts
	operator VkInstance () const { return inst_; }
};
#endif