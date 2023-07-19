

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_structs.hpp>
namespace plt 
{

Result<> VkGfx::createDescriptorSetLayout()
{

	info$("Creating descriptor set layout");

	auto uboLayoutBinding = vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	auto samplerLayoutBinding = vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(1)
		.setPBindings(&uboLayoutBinding);

	vkTry$(this->LogicalDevice.createDescriptorSetLayout(&samplerLayoutBinding, nullptr, &this->descriptorSetLayout));

	this->deinit_funcs.push_back([](VkGfx *gfx) {
		gfx->LogicalDevice.destroyDescriptorSetLayout(gfx->descriptorSetLayout);
	});

	return {};
}
}
