#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#include "common/vk_instance.h"

int main(int argc, char **argv)
{
	{
		vk_instance default_instance();
	}

	{
		vk_instance debug_instance(
		{},
		{ VK_EXT_DEBUG_REPORT_EXTENSION_NAME });
	}

	{
		vk_instance extended_instance(
			{},
			{
				VK_KHR_SURFACE_EXTENSION_NAME,
				VK_EXT_DEBUG_REPORT_EXTENSION_NAME
			}
		);
	}

	return 0;
}