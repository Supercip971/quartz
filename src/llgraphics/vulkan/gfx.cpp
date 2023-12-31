
#include <llgraphics/gfx.hpp>
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"

#include "utils/log.hpp"
#include "window/window.hpp"
namespace plt {

static VkGfx instance;
Gfx *Gfx::the() {
    return &instance;
};

Result<> Gfx::init() {

    debug$("initializing gfx");
    instance = VkGfx();
    try$(instance.setupVulkan());

    return {};
}

Result<> Gfx::attach(plt::Window &target) {
    debug$("attaching to window: {}", target.handle());

    try$(instance.attachVulkan(&target));

    return {};
}

Result<> Gfx::resize(plt::Window &target) {
    debug$("resizing window: {}", target.handle());

    try$(instance.resizeSwapchain(&target));

    return {};
}
void Gfx::cleanup(void) {
    info$("cleaning up gfx");
    instance.vulkanDeinit();
}
Result<> Gfx::render(void) {

    try$(instance.drawFrame());
    return {};
}

}; // namespace plt
