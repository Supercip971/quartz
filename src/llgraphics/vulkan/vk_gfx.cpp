
#include <llgraphics/gfx.hpp>
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "llgraphics/vulkan/cmd/buffer.hpp"
#include "utils/log.hpp"
#include "window/window.hpp"
namespace plt {

Result<> VkGfx::setupVulkan() {
    info$("setting up vulkan");

    try$(createInstance());
    return {};
}

Result<> VkGfx::attachVulkan(Window *window) {
    info$("attaching vulkan to window");

    try$(setupSurface(window));
    try$(pickPhysicalDevice());
    try$(setupLogicalDevice());
    try$(setupSwapchain(window));
    try$(createImageViews());
    try$(createShaderPipeline());
    try$(createRenderPass());
    try$(createGraphicPipeline());
    try$(createFramebuffers());
    try$(createCommandPool());
    try$(createCommandBuffers());
    try$(createSyncObjects());

    return {};
};

/*
 *
 * Semaphore:
 * - A semaphore is used to add order between queue operations.
 * - We will use semaphore as a shared state, one command will signal a semaphore and another one will wait the semaphore.
 *
 * Fence
 * - A way to synchronize between CPU and GPU.
 */

Result<> VkGfx::drawFrame() {

    uint32_t imageIndex;

    vkTry$(this->LogicalDevice.acquireNextImageKHR(this->swapchain, UINT64_MAX, this->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex));

    std::vector<vk::Fence> waitFences = {this->inFlightFence};
    vkTry$(this->LogicalDevice.waitForFences(waitFences, VK_TRUE, UINT64_MAX));

    this->LogicalDevice.resetFences(waitFences);

    this->cmdBuffer->reset();

    this->recordRenderCommands(imageIndex);

    std::vector<vk::PipelineStageFlags> waitStages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    std::vector<vk::Semaphore> waitSemaphores = {this->imageAvailableSemaphore};
    std::vector<vk::CommandBuffer> commandBuffers = {this->cmdBuffer.buf()};
    std::vector<vk::Semaphore> signalSemaphores = {this->renderFinishedSemaphore};

    // we will wait until trying to write the image that the 'ImageAvailableSemaphore' is signaled
    // then we will signal the 'RenderFinishedSemaphore' when we are done writing the image
    auto submitInfo = vk::SubmitInfo()
                          .setWaitSemaphores(waitSemaphores)
                          .setWaitDstStageMask(waitStages)
                          .setCommandBuffers(commandBuffers)
                          .setSignalSemaphores(signalSemaphores);

    (this->graphicsQueue.submit(submitInfo, this->inFlightFence));

    vk::SwapchainKHR swapchains[] = {this->swapchain};
    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
                                         .setWaitSemaphores(signalSemaphores)
                                         .setSwapchains(swapchains)
                                         .setImageIndices(imageIndex);

    vkTry$(presentQueue.presentKHR(presentInfo));

    return {};
}

}; // namespace plt
