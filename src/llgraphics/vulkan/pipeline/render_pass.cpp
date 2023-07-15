
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

    // the first subpass (here: 0) needs to wait to the swapchain to finish reading from the image
    //  thats why we use VK_SUBPASS_EXTERNAL
    //
    auto depencency = vk::SubpassDependency()
                          .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                          .setDstSubpass(0)
                          // wait for the pipeline to finish reading from the image (swapchain)
                          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                          .setSrcAccessMask(vk::AccessFlags())
                          // wait for the pipeline to finish writing to the image (swapchain)
                          .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                          .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    std::vector<vk::SubpassDependency> dependencies = {depencency};

    auto renderPassInfo = vk::RenderPassCreateInfo()
                              .setAttachments(attachments)
                              .setSubpasses(subpass)
                              .setDependencies(dependencies);

    vkTry$(this->LogicalDevice.createRenderPass(&renderPassInfo, nullptr, &this->renderPass));

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        gfx->LogicalDevice.destroyRenderPass(gfx->renderPass);
        debug$("render pass destroyed");
    });

    return {};
}

} // namespace plt
