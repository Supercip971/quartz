
#include <llgraphics/gfx.hpp>
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "utils/log.hpp"
#include "validation-layers.hpp"
#include "window/window.hpp"
namespace plt {

Result<> VkGfx::createInstance() {
    debug$("creating instance");

    vk::ApplicationInfo appInfo =
        vk::ApplicationInfo("yavp",
                            1,
                            "yavp",
                            1,
                            VK_API_VERSION_1_2);

    std::vector<char const *> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

    auto window_exts = plt::Window::required_extensions();
    for (auto &ext : window_exts) {
        extensions.push_back(ext);
    }

    std::vector<char const *>
        layers = {};

    if (enableValidationLayers) {
        if (!checkDriverValidationLayers()) {
            warn$("trying to enable validation layers, but they are not supported by the driver");
        } else {
            info$("enabling validation layers");
            layers = getValidationLayers();
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
    }

    auto validationLayerInfo = plt::debugMessengerCreationInfo();
    vk::InstanceCreateInfo createInfo =
        vk::InstanceCreateInfo({},
                               &appInfo,
                               layers,
                               extensions,
                               (enableValidationLayers) ? &validationLayerInfo : nullptr);

    vkTry$(vk::createInstance(
        &createInfo,
        nullptr,
        &instance));

    debug$("created instance");

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        debug$("destroying instance");
        gfx->instance.destroy();
    });

    debug$("enabling validation layer for instance");

    if (enableValidationLayers) {
        debug$("setting up debug messenger");
        setupDebugMessanger();
    }
    return {};
}
}; // namespace plt
