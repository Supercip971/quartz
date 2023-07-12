#pragma once

#include <vulkan/vulkan.hpp>

#include <vulkan/vulkan_handles.hpp>

#include "llgraphics/gfx.hpp"
#include "llgraphics/vulkan/device/physical.hpp"
#include "utils/traits.hpp"

namespace plt {

class VkGfx : public NoCopy, public Gfx {

public:
    Result<> setupVulkan();
    Result<> attachVulkan(Window *window);

    Result<> createInstance();

    Result<> setupDebugMessanger();

    Result<> setupSurface(Window *window);

    Result<> pickPhysicalDevice();

    Result<> setupLogicalDevice();

    QueueFamilyIndices findPhysicalDeviceQueueFamily();

    void vulkanDeinit() {

        for (auto it = this->deinit_funcs.rbegin(); it != this->deinit_funcs.rend(); ++it) {
            (*it)(this);
        }

        this->deinit_funcs.clear();

        info$("vulkan deinitilized");
    }

	QueueFamilyIndices findPhysicalDeviceQueueFamily(vk::PhysicalDevice device);

	bool isDeviceSuitable(vk::PhysicalDevice device);

private:
    std::vector<std::function<void(VkGfx *)>> deinit_funcs;

    /* in instance */
    vk::Instance instance;
    vk::DebugUtilsMessengerEXT debugMessenger;

    /* surface */

    vk::SurfaceKHR surface;
    /* in device */
    vk::PhysicalDevice physicalDevice;
    vk::Device LogicalDevice;

    vk::Queue graphicsQueue;
	vk::Queue presentQueue;
};

[[maybe_unused]] static inline Result<> vkTry(vk::Result res) {
    if (res != vk::Result::eSuccess) {
        return Result<>::err("vulkan error: {}", (uint32_t)res);
    }
    return {};
}

#define vkTry$(r) try$(vkTry(r))

} // namespace plt
