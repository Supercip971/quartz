
#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_structs.hpp>
namespace plt 
{


Result<> VkGfx::createImageViews()
{

	debug$("creating image views");

	this->swapchainImageViews.resize(this->swapchainImages.size());

	for(size_t i = 0; i < swapchainImages.size(); i++)
	{

		debug$("creating image view for image {}", i);
		vk::ImageViewCreateInfo createInfo = vk::ImageViewCreateInfo()
			.setImage(swapchainImages[i])
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(swapchainImageFormat)
			.setComponents(vk::ComponentMapping()
				.setR(vk::ComponentSwizzle::eIdentity)
				.setG(vk::ComponentSwizzle::eIdentity)
				.setB(vk::ComponentSwizzle::eIdentity)
				.setA(vk::ComponentSwizzle::eIdentity))
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1));

		vkTry$(this->LogicalDevice.createImageView(&createInfo, nullptr, &this->swapchainImageViews[i]));

		this->deinit_funcs.push_back([=](VkGfx *gfx) {
			debug$("destroying image view: {}", i);
			gfx->LogicalDevice.destroyImageView(gfx->swapchainImageViews[i]);
		});


	}
	return {};
}

}
