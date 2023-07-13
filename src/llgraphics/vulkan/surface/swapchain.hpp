
#pragma once

#include <vulkan/vulkan.hpp>

namespace plt {

struct SwapchainSupportInfos {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;

    vk::SurfaceFormatKHR chooseFormat() const;
    vk::PresentModeKHR choosePresentMode() const;

    vk::Extent2D chooseExtent(int width, int height) const;
};

} // namespace plt
