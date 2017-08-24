#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <vulkan/vulkan.h>

#include "common/utils.h"
#include "common/vk_instance.h"

int main(int argc, char **argv)
{
	vk_instance				inst;

	VkPhysicalDeviceProperties query_pdp = { 0 };
	query_pdp.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

	auto ideal_device = find_matching_devices (inst, query_pdp, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);

	return 0;
}