

#include "llgraphics/vulkan/vk_gfx.hpp"
namespace plt {

Result<> VkGfx::createCommandPool() {
    debug$("creating command pool...");

    auto queueFamilyIndices = this->findPhysicalDeviceQueueFamily();

    auto poolInfo = vk::CommandPoolCreateInfo()
                        .setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.index)
                        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    vkTry$(this->LogicalDevice.createCommandPool(&poolInfo, nullptr, &this->commandPool));

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        debug$("destroying command pool...");

        gfx->LogicalDevice.destroyCommandPool(gfx->commandPool);
    });

    return {};
}
} // namespace plt
