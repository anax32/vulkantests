#ifndef VK_SHADER_MODULE_H
#define VK_SHADER_MODULE_H
class vk_shader_module
{
public:
	typedef std::vector<uint32_t>	spir_v_buffer;

	static auto read_spir_v_file(std::string filename)->spir_v_buffer
	{
		std::ifstream	ifs(filename.c_str(), std::ios::in | std::ios::binary);
		spir_v_buffer	contents;

		if (ifs.is_open() == true)
		{
			ifs.seekg(0, std::ios::end);
			auto sz = ifs.tellg();
			ifs.seekg(0, std::ios::beg);
			contents.resize(sz / sizeof(vk_shader_module::spir_v_buffer::value_type));
			ifs.read((char*)contents.data(), sz);
			ifs.close();
		}

		return contents;
	}

protected:
	VkDevice		device_;
	VkShaderModule	module_;

public:
	vk_shader_module(VkDevice device, spir_v_buffer source)
		: device_(device)
	{
		// create the shader
		VkShaderModuleCreateInfo	shader_info;
		shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shader_info.pNext = NULL;
		shader_info.flags = 0;
		shader_info.codeSize = source.size() * sizeof(spir_v_buffer::value_type);
		shader_info.pCode = source.data();

		auto res = vkCreateShaderModule(device_, &shader_info, NULL, &module_);
	}
	// execute the shader
	//	vkCmdDispatch();
	virtual ~vk_shader_module()
	{
		// cleanup of shader
		vkDestroyShaderModule(device_, module_, NULL);
	}
	operator VkShaderModule () const { return module_; }
};
#endif