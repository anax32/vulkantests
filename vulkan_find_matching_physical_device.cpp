#ifdef WIN32
#pragma comment(lib, "vulkan-1.lib")
#endif

#include <iostream>

#include <vulkan/vulkan.h>

#include "common/utils.h"
#include "common/vk_instance.h"

int main(int argc, char **argv)
{
	vk_instance				inst;

	VkPhysicalDeviceProperties query_pdp = { 0 };
	query_pdp.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

	auto ideal_devices = find_matching_devices (inst, query_pdp, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);

  if (ideal_devices.empty () == false)
  {
    std::cout << "Found " << ideal_devices.size () << " discrete GPU with local memory" << std::endl;
  }
  else
  {
    std::cout << "Could not find discrete GPU with local memory" << std::endl;
  }

	return 0;
}
