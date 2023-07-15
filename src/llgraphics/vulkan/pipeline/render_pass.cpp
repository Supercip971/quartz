
#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_structs.hpp>
namespace plt {

Result<> VkGfx::createRenderPass() {
    debug$("creating render pass...");
    auto colorAttachment = vk::AttachmentDescription()
                               .setFormat(this->swapchainImageFormat)
                               .setSamples(vk::SampleCountFlagBits::e1)
                               // color and depth data
                               .setLoadOp(vk::AttachmentLoadOp::eClear)
                               .setStoreOp(vk::AttachmentStoreOp::eStore)
                               // stencil data
                               .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                               .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                               .setInitialLayout(vk::ImageLayout::eUndefined)
                               .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    std::vector<vk::AttachmentDescription> attachments = {colorAttachment};

    // reference color attachment
    auto colorAttachmentRef = vk::AttachmentReference()
                                  .setAttachment(0) // attachments[0]
                                  .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    std::vector<vk::AttachmentReference> attachmentRefs = {colorAttachmentRef};

    auto subpass = vk::SubpassDescription()
                       .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                       .setColorAttachments(colorAttachmentRef);

    auto renderPassInfo = vk::RenderPassCreateInfo()
                              .setAttachments(attachments)
                              .setSubpasses(subpass);

    vkTry$(this->LogicalDevice.createRenderPass(&renderPassInfo, nullptr, &this->renderPass));

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        gfx->LogicalDevice.destroyRenderPass(gfx->renderPass);
        debug$("render pass destroyed");
    });

    return {};
}

} // namespace plt
