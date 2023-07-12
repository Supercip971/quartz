
#include "llgraphics/vulkan/device/physical.hpp"

#include "llgraphics/vulkan/vk_gfx.hpp"

namespace plt {

struct ScoredDevice {
    vk::PhysicalDevice device;
    float score;
};

float deviceScore(vk::PhysicalDevice device) {
    auto props = device.getProperties();
    [[maybe_unused]] auto features = device.getFeatures();

    float score = 0.0f;

    if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000.0f;
    }

    score += props.limits.maxImageDimension2D;

    return score;
}

QueueFamilyIndices VkGfx::findPhysicalDeviceQueueFamily() {
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();

    QueueFamilyIndices result;

    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            result = QueueFamilyIndices(i);
        }
    }

    return result;
}

QueueFamilyIndices findPhysicalDeviceQueueFamily(vk::PhysicalDevice device) {
    auto queueFamilies = device.getQueueFamilyProperties();

    QueueFamilyIndices result;

    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            result = QueueFamilyIndices(i);
        }
    }

    return result;
}

bool isDeviceSuitable(vk::PhysicalDevice device) {
    // auto props = device.getProperties();
    auto features = device.getFeatures();

    auto indices = findPhysicalDeviceQueueFamily(device);

    return features.geometryShader && indices.isComplete();
}

Result<> VkGfx::pickPhysicalDevice() {

    debug$("selecting vulkan physical device");

    auto devices = this->instance.enumeratePhysicalDevices();

    if (devices.empty()) {
        return Result<>::err("no vulkan devices found");
    }

    std::vector<ScoredDevice> scoredDevices;

    int i = 0;
    for (auto &device : devices) {
        debug$("device[{}]: {}", i++, device.getProperties().deviceName);
        debug$("- score: {}", deviceScore(device));
        debug$("- suitable: {}", isDeviceSuitable(device));

        if (isDeviceSuitable(device)) {
            scoredDevices.push_back({device, deviceScore(device)});
        }
    }

    if (scoredDevices.empty()) {
        return Result<>::err("no suitable vulkan devices found");
    }

    std::sort(scoredDevices.begin(), scoredDevices.end(), [](auto &a, auto &b) {
        return a.score > b.score;
    });

    this->physicalDevice = scoredDevices[0].device;

    debug$("selected device: {}", physicalDevice.getProperties().deviceName);

    return {};
}
} // namespace plt
