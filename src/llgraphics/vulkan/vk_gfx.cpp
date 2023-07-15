
#include <llgraphics/gfx.hpp>
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"
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

    return {};
}

}; // namespace plt
