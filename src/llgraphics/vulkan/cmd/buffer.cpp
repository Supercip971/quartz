
#include "llgraphics/vulkan/cmd/buffer.hpp"
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"
namespace plt {

Result<> VkGfx::createCommandBuffers() {
    debug$("creating command buffer...");

    this->cmdBuffer.resize(MAX_FRAMES_IN_FLIGHT);

    this->cmdBuffer = try$(VkCmdBuffer::create_buffers(this->LogicalDevice, this->commandPool, MAX_FRAMES_IN_FLIGHT));

    return {};
}

Result<> VkGfx::recordRenderCommands(VkCmdBuffer &target, uint32_t imageIndex) {


    target.start();

    {

        vk::ClearValue clearValues = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
        auto beginInfo = vk::RenderPassBeginInfo()
                             .setRenderPass(this->renderPass)
                             .setFramebuffer(this->swapchainFramebuffers[imageIndex])
                             .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), this->swapchainExtent))
                             .setClearValueCount(1)
                             .setPClearValues(&clearValues);

        target->beginRenderPass(beginInfo, vk::SubpassContents::eInline);

        target->bindPipeline(vk::PipelineBindPoint::eGraphics, this->graphicPipeline);
		

		std::vector<vk::Buffer> bufs = {this->mesh.VertexBuffer()};
		std::vector<vk::DeviceSize> offsets = {0};
		target->bindVertexBuffers(0, 1, bufs.data(), offsets.data());

        auto viewport = vk::Viewport(0.0f, 0.0f, (float)this->swapchainExtent.width, (float)this->swapchainExtent.height, 0.0f, 1.0f);
        target->setViewport(0, 1, &viewport);
        vk::Rect2D scissor(vk::Offset2D(0, 0), this->swapchainExtent);

        target->setScissor(0, 1, &scissor);

        target->draw(this->mesh.verticesCount(), 1, 0, 0);

        target->endRenderPass();
    }

    target.end();

    return {};
}

} // namespace plt
