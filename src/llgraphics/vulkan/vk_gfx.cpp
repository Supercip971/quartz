
#include <llgraphics/gfx.hpp>
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_structs.hpp>

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
    try$(createGraphicPipeline());

    return {};
};

}; // namespace plt
