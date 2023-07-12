
#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_structs.hpp>
#include <set>
namespace plt {

Result<> VkGfx::setupLogicalDevice() {

    debug$("creating logical device...");

    auto queueFamilies = this->findPhysicalDeviceQueueFamily();

    std::vector<float> queuePriorities = {1.0f};

	std::set<int> uniqueQueueFamilies = {
		queueFamilies.graphicsFamily.index,
		queueFamilies.presentFamily.index
	};

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	for(auto idx : uniqueQueueFamilies) {
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo()
									.setQueueFamilyIndex(idx)
									.setQueueCount(1)
									.setPQueuePriorities(queuePriorities.data()));
	}
	


    vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

    vk::DeviceCreateInfo info = vk::DeviceCreateInfo()
                                    .setQueueCreateInfos(queueCreateInfos)
                                    .setPEnabledFeatures(&deviceFeatures);

    try$(vkTry(this->physicalDevice.createDevice(&info, nullptr, &this->LogicalDevice)));

    debug$("created logical device");

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        debug$("destroying logical device");
        gfx->LogicalDevice.destroy();
    });

    this->graphicsQueue = this->LogicalDevice.getQueue(queueFamilies.graphicsFamily, 0);
	this->presentQueue = this->LogicalDevice.getQueue(queueFamilies.presentFamily, 0);
    return {};
}
} // namespace plt
