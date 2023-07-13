
#include <cstdint>

#include "llgraphics/vulkan/vk_gfx.hpp"

#include "llgraphics/vulkan/surface/swapchain.hpp"

namespace plt {
Result<> VkGfx::setupSwapchain(Window *window) {

    debug$("creating swapchain");

    auto supportInfo = querySwapchainSupport(this->physicalDevice);
    auto surfaceFormat = supportInfo.chooseFormat();
    auto presentMode = supportInfo.choosePresentMode();
    auto extent = supportInfo.chooseExtent(window->fb_width(), window->fb_height());

    debug$("- surface format: {}", (uint32_t)surfaceFormat.format);
    debug$("- present mode: {}", (uint32_t)presentMode);
    debug$("- extent: {}x{} (fb: {}x{})", extent.width, extent.height, window->fb_width(), window->fb_height());

    auto image_count = supportInfo.capabilities.minImageCount + 1;
    if (supportInfo.capabilities.maxImageCount > 0 && image_count > supportInfo.capabilities.maxImageCount) {
        image_count = supportInfo.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR()
                                                .setSurface(surface)
                                                .setMinImageCount(image_count)
                                                .setImageFormat(surfaceFormat.format)
                                                .setImageColorSpace(surfaceFormat.colorSpace)
                                                .setImageExtent(extent)
                                                .setImageArrayLayers(1)
                                                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                                                .setPreTransform(supportInfo.capabilities.currentTransform)
                                                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                                                .setPresentMode(presentMode)
                                                .setClipped(true);

    auto indices = this->queueFamilyIndices;

    std::vector<uint32_t> queueFamilyIndices = {(uint32_t)indices.graphicsFamily.index, (uint32_t)indices.presentFamily.index};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndexCount(2)
            .setQueueFamilyIndices(queueFamilyIndices);
    } else {
        createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
    }

    try$(vkTry(this->LogicalDevice.createSwapchainKHR(&createInfo, nullptr, &this->swapchain)));

    debug$("created swapchain");

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        debug$("destroying swapchain");
        gfx->LogicalDevice.destroySwapchainKHR(gfx->swapchain);
    });

    debug$("getting swapchain images");
    this->swapchainImages = this->LogicalDevice.getSwapchainImagesKHR(this->swapchain);

    this->swapchainImageFormat = surfaceFormat.format;
    this->swapchainExtent = extent;

    return {};
}

SwapchainSupportInfos VkGfx::querySwapchainSupport(vk::PhysicalDevice device) {
    SwapchainSupportInfos infos = {};

    infos.capabilities = device.getSurfaceCapabilitiesKHR(this->surface);
    infos.formats = device.getSurfaceFormatsKHR(this->surface);
    infos.presentModes = device.getSurfacePresentModesKHR(this->surface);

    return infos;
}
vk::SurfaceFormatKHR SwapchainSupportInfos::chooseFormat() const {

    for (auto const &availableFormat : formats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return formats[0];
};

vk::PresentModeKHR SwapchainSupportInfos::choosePresentMode() const {
    for (auto const &availablePresentMode : presentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
};
vk::Extent2D SwapchainSupportInfos::chooseExtent(int width, int height) const {

    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {

        vk::Extent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.height, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.width, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

} // namespace plt
