#include "vk_result.h"
#include <cstring>

#ifndef VK_MEMORY_ALLOCATION_H
#define VK_MEMORY_ALLOCATION_H
class vk_memory_allocation : public vk_result
{
protected:
	VkDevice		device_;
	VkDeviceMemory	mem_;
public:
	vk_memory_allocation(VkDevice device, VkDeviceSize size, uint32_t memory_type_index)
		: device_(device),
		  mem_(VK_NULL_HANDLE)
	{
		// attempt to allocate some memory
		VkMemoryAllocateInfo	alloc_info;
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.pNext = NULL;
		alloc_info.allocationSize = size;
		alloc_info.memoryTypeIndex = memory_type_index;

		result = vkAllocateMemory(
			device_,
			&alloc_info,
			NULL,
			&mem_);
	}
	virtual ~vk_memory_allocation()
	{
		vkFreeMemory(
			device_,
			mem_,
			NULL);
	}
	operator VkDeviceMemory () const { return mem_; }

	char* map (const VkDeviceSize length)
	{
		char *ptr = NULL;

		result = vkMapMemory(
			device_,
			mem_,
			0,
			length,
			VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_HOST_BIT,
			(void**)&ptr);

		return ptr;
	}
	void unmap()
	{
		vkUnmapMemory(
			device_,
			mem_);
	}

	template<typename T>
	void put(T* in, const size_t num_elements)
	{
		//if (std::distance(begin, end) > MEMSIZE)		{error}
		// if memtype != HOST_MEMORY or HOST_VISIBLE	{error}
		auto ptr = map (num_elements*sizeof(T));
		//std::copy(begin, end, ptr);
		std::memcpy(ptr, in, num_elements*sizeof(T));
		unmap();
	}
	template<typename T>
	void get(T* out, const size_t num_elements)
	{
		//if (std::distance(begin, end) > MEMSIZE)		{error}
		// if memtype != HOST_MEMORY or HOST_VISIBLE	{error}
		auto ptr = map (num_elements*sizeof(T));
		//std::copy(ptr, ptr + len, begin);
		std::memcpy(out, ptr, num_elements*sizeof(T));
		unmap();
	}
};
#endif