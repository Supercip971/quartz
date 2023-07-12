
#include "llgraphics/vulkan/vk_gfx.hpp"

#include "window/window.hpp"
namespace plt {
Result<> VkGfx::setupSurface(plt::Window *window) {

    debug$("creating surface");
    auto res = try$(window->create_surface(this->instance));

    this->surface = res;

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        debug$("destroying surface");
        gfx->instance.destroySurfaceKHR(gfx->surface);
    });

    return {};
}
} // namespace plt
