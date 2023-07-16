
#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_handles.hpp>
namespace plt {

Result<> VkGfx::createFramebuffers(bool recreated) {

    this->swapchainFramebuffers.resize(this->swapchainImageViews.size());

    for (size_t i = 0; i < this->swapchainImageViews.size(); i++) {
        debug$("creating framebuffer: {}", i);

        std::vector<vk::ImageView> attachments = {this->swapchainImageViews[i]};

        auto framebufferInfo = vk::FramebufferCreateInfo()
                                   .setRenderPass(this->renderPass)
                                   .setAttachments(attachments)
                                   .setWidth(this->swapchainExtent.width)
                                   .setHeight(this->swapchainExtent.height)
                                   .setLayers(1);

        vkTry$(this->LogicalDevice.createFramebuffer(&framebufferInfo, nullptr, &this->swapchainFramebuffers[i]));

        if (!recreated) {

            this->deinit_funcs.push_back([i](VkGfx *gfx) {
                gfx->LogicalDevice.destroyFramebuffer(gfx->swapchainFramebuffers[i]);
                debug$("framebuffer destroyed: {}", i);
            });
        }
    }

    return {};
}

} // namespace plt
