
#include <llgraphics/gfx.hpp>
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "llgraphics/vulkan/cmd/buffer.hpp"
#include "llgraphics/vulkan/mesh/mesh.hpp"
#include "llgraphics/vulkan/mesh/vertex.hpp"
#include "utils/log.hpp"
#include "window/window.hpp"
namespace plt {

Result<> VkGfx::setupVulkan() {
    info$("setting up vulkan");

    try$(createInstance());
    return {};
}

Result<> VkGfx::resizeSwapchain(Window *window) {
    info$("resizing swapchain: {}x{}", window->width(), window->height());
    this->invalidatedSwapchain = true;
    return {};
}

std::vector<NVertex> points = {
    NVertex({0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}),
    NVertex({0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}),
    NVertex({-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}),
};
Result<> VkGfx::attachVulkan(Window *window) {
    info$("attaching vulkan to window");

    this->mesh = plt::VkMesh<>();
    this->window = window;
    try$(setupSurface(window));
    try$(pickPhysicalDevice());
    try$(setupLogicalDevice());
    try$(setupSwapchain(window));
    try$(createImageViews());
    try$(createShaderPipeline());
    try$(createRenderPass());
    try$(createGraphicPipeline(this->mesh.description()));
    try$(createFramebuffers());
    try$(createCommandPool());
    try$(createCommandBuffers());
    try$(createSyncObjects());

    this->mesh = plt::VkMesh<>(points.data(), points.size());
    this->mesh.allocateGpuBuffers(this->ctx());

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        gfx->mesh.releaseGpuBuffers();
    });

    return {};
};

Result<bool> VkGfx::recreateSwapchainIfNecessary(vk::Result res, bool noSuboptimal) {
    if (invalidatedSwapchain && !noSuboptimal) {
        try$(recreateSwapchain(this->window));
        return {true};
    } else if (res == vk::Result::eErrorOutOfDateKHR || (res == vk::Result::eSuboptimalKHR)) {

        if (noSuboptimal && res == vk::Result::eSuboptimalKHR) {
            return {false};
        }

        try$(recreateSwapchain(this->window));
        return {true};
    } else if (res != vk::Result::eSuccess) {
        error$("failed to acquire swapchain image");
        vkTry$(res);
        return {false};
    }
    return {false};
}
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

    std::vector<vk::Fence> waitFences = {this->inFlightFences[currentFrame]};
    vkTry$(this->LogicalDevice.waitForFences(waitFences, VK_TRUE, UINT64_MAX));
    auto res = (this->LogicalDevice.acquireNextImageKHR(this->swapchain, UINT64_MAX, this->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex));

    if (try$(recreateSwapchainIfNecessary(res, true))) {
        return {};
    }

    this->LogicalDevice.resetFences(waitFences);

    this->cmdBuffer[currentFrame]->reset();

    this->recordRenderCommands(cmdBuffer[currentFrame], imageIndex);

    std::vector<vk::PipelineStageFlags> waitStages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    std::vector<vk::Semaphore> waitSemaphores = {this->imageAvailableSemaphores[currentFrame]};
    std::vector<vk::CommandBuffer> commandBuffers = {this->cmdBuffer[currentFrame].buf()};
    std::vector<vk::Semaphore> signalSemaphores = {this->renderFinishedSemaphores[currentFrame]};

    // we will wait until trying to write the image that the 'ImageAvailableSemaphore' is signaled
    // then we will signal the 'RenderFinishedSemaphore' when we are done writing the image
    auto submitInfo = vk::SubmitInfo()
                          .setWaitSemaphores(waitSemaphores)
                          .setWaitDstStageMask(waitStages)
                          .setCommandBuffers(commandBuffers)
                          .setSignalSemaphores(signalSemaphores);

    (this->graphicsQueue.submit(submitInfo, this->inFlightFences[currentFrame]));

    vk::SwapchainKHR swapchains[] = {this->swapchain};
    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
                                         .setWaitSemaphores(signalSemaphores)
                                         .setSwapchains(swapchains)
                                         .setImageIndices(imageIndex);

    res = (presentQueue.presentKHR(presentInfo));

    try$(recreateSwapchainIfNecessary(res, false));
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    return {};
}

}; // namespace plt
