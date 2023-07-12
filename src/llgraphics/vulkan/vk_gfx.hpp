#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "llgraphics/gfx.hpp"
#include "utils/traits.hpp"

namespace plt {

class VkGfx : public NoCopy, public Gfx {

public:
    Result<> setupVulkan();

    Result<> createInstance();

    Result<> setupDebugMessanger();

    void vulkanDeinit() {

        for (auto it = this->deinit_funcs.rbegin(); it != this->deinit_funcs.rend(); ++it) {
            (*it)(this);
        }

        this->deinit_funcs.clear();

        info$("vulkan deinitilized");
    }

private:
    std::vector<std::function<void(VkGfx *)>> deinit_funcs;

    vk::Instance instance;
	vk::DebugUtilsMessengerEXT debugMessenger;
};

[[maybe_unused]] static inline Result<> vkTry(vk::Result res) {
    if (res != vk::Result::eSuccess) {
        return Result<>::err("vulkan error: {}", (uint32_t)res);
    }
    return {};
}

#define vkTry$(r) try$(vkTry(r))

} // namespace plt
