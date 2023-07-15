
#include "llgraphics/vulkan/vk_gfx.hpp"
namespace plt {

Result<> VkGfx::createCommandBuffers() {
    debug$("creating command buffer...");
    this->cmdBuffer = try$(VkCmdBuffer::create(this->LogicalDevice, this->commandPool));

    return {};
}

Result<> VkGfx::recordRenderCommands(uint32_t imageIndex) {

    this->cmdBuffer.start();

    {

        vk::ClearValue clearValues = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
        auto beginInfo = vk::RenderPassBeginInfo()
                             .setRenderPass(this->renderPass)
                             .setFramebuffer(this->swapchainFramebuffers[imageIndex])
                             .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), this->swapchainExtent))
                             .setClearValueCount(1)
                             .setPClearValues(&clearValues);

        this->cmdBuffer->beginRenderPass(beginInfo, vk::SubpassContents::eInline);

        this->cmdBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, this->graphicPipeline);

        auto viewport = vk::Viewport(0.0f, 0.0f, (float)this->swapchainExtent.width, (float)this->swapchainExtent.height, 0.0f, 1.0f);
        this->cmdBuffer->setViewport(0, 1, &viewport);
        vk::Rect2D scissor(vk::Offset2D(0, 0), this->swapchainExtent);

        this->cmdBuffer->setScissor(0, 1, &scissor);

        this->cmdBuffer->draw(3, 1, 0, 0);

        this->cmdBuffer->endRenderPass();
    }

    this->cmdBuffer.end();

    return {};
}

} // namespace plt
