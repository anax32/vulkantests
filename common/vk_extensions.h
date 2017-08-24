#ifndef vk_extension_h
#define vk_extension_h
class vk_extension_base
{
protected:
	VkInstance inst_;
public:
	vk_extension_base(VkInstance instance)
		: inst_(instance)
	{}
	virtual ~vk_extension_base()
	{
		std::cout << "~vk_extension_base" << std::endl;
	}
};
class VK_KHR_surface_extension : public vk_extension_base
{
public:
	VK_KHR_surface_extension(VkInstance instance)
		: vk_extension_base(instance)
	{}
	virtual ~VK_KHR_surface_extension()
	{
		std::cout << "VK_KHR_surface_extension destroyed" << std::endl;
	}
	operator const char *()			{ return "VK_KHR_surface"; }
};
class vk_debug_extension : public vk_extension_base
{
public:
	static VkBool32 callback(VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objectType,
		uint64_t object,
		size_t location,
		int32_t messageCode,
		const char* pLayerPrefix,
		const char * pMessage,
		void* pUserData)
	{
		std::cout << pMessage << std::endl;
		return VK_FALSE;	// do not abort the call
	}
protected:
	VkDebugReportCallbackEXT	callback_handle_;

	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;

	void get_fn_ptrs()
	{
		vkCreateDebugReportCallbackEXT =
			reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
			(vkGetInstanceProcAddr(inst_, "vkCreateDebugReportCallbackEXT"));
		vkDebugReportMessageEXT =
			reinterpret_cast<PFN_vkDebugReportMessageEXT>
			(vkGetInstanceProcAddr(inst_, "vkDebugReportMessageEXT"));
		vkDestroyDebugReportCallbackEXT =
			reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
			(vkGetInstanceProcAddr(inst_, "vkDestroyDebugReportCallbackEXT"));
	}
public:
	vk_debug_extension(VkInstance instance)
		: vk_extension_base(instance)
	{
		get_fn_ptrs();

		VkDebugReportCallbackCreateInfoEXT create_info =
		{
			VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
			NULL,
			VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
			VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_DEBUG_BIT_EXT,
			(PFN_vkDebugReportCallbackEXT)callback,
			NULL
		};

		auto res = vkCreateDebugReportCallbackEXT(
			inst_,
			&create_info,
			NULL,
			&callback_handle_);
	}
	void print(const char *msg)
	{
		vkDebugReportMessageEXT(
			inst_,
			0,
			VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT,
			VK_NULL_HANDLE,
			0,
			0,
			NULL,
			msg);
	}
	virtual ~vk_debug_extension()
	{
		vkDestroyDebugReportCallbackEXT(
			inst_,
			callback_handle_,
			NULL);
	}
	operator VkDebugReportCallbackEXT () const { return callback_handle_; }
	operator const char *() { return "VK_EXT_debug_report\0"; }
};
#endif