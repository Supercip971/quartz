
#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_structs.hpp>
namespace plt {

Result<> VkGfx::setupLogicalDevice() {

    debug$("creating logical device...");

    auto queueFamilies = this->findPhysicalDeviceQueueFamily();

    std::vector<float> queuePriorities = {1.0f};
    vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
                                                    .setQueueFamilyIndex(queueFamilies.graphicsFamily)
                                                    .setQueueCount(1)
                                                    .setPQueuePriorities(queuePriorities.data());

    vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

    vk::DeviceCreateInfo info = vk::DeviceCreateInfo()
                                    .setQueueCreateInfoCount(1)
                                    .setPQueueCreateInfos(&queueCreateInfo)
                                    .setPEnabledFeatures(&deviceFeatures);

    try$(vkTry(this->physicalDevice.createDevice(&info, nullptr, &this->LogicalDevice)));

    debug$("created logical device");

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        debug$("destroying logical device");
        gfx->LogicalDevice.destroy();
    });

    this->graphicsQueue = this->LogicalDevice.getQueue(queueFamilies.graphicsFamily, 0);
    return {};
}
} // namespace plt
