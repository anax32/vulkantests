#ifndef VK_RESULT_H
#define VK_RESULT_H
class vk_result
{
protected:
	VkResult	result;

public:
	vk_result()
		: result(VK_RESULT_MAX_ENUM)
	{}
	virtual ~vk_result()
	{}

	void print_result()
	{
		switch (result)
		{
			case VK_SUCCESS:
			{
				break;
			}
			case VK_ERROR_INCOMPATIBLE_DRIVER:
			{
#ifdef _IOSTREAM_
				std::cout << "VK_ERROR_INCOMPATIBLE_DRIVER" << std::endl;
				// FIXME: compiler warning about omitted output
#endif
				break;
			}
			default:
			{
#ifdef _IOSTREAM_
				std::cout << "unknown error" << result << std::endl;
				// FIXME: compiler warning about omitted output
#endif
			}
		}
	}

	operator VkResult () const { return result; }
};
#endif