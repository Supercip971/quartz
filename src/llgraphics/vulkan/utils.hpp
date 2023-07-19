#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "utils/result.hpp"

namespace plt {

[[maybe_unused]] static inline Result<> vkTry(vk::Result res) {
    if (res != vk::Result::eSuccess) {
        return Result<>::err("vulkan error: {}", (uint32_t)res);
    }
    return {};
};

[[maybe_unused]] static inline Result<> vkTry(VkResult res) {
    if (res != VK_SUCCESS) {
        return Result<>::err("vulkan error: {}", (uint32_t)res);
    }
    return {};
};


#define vkTry$(r) try$(vkTry(r))

struct GpuCtx {
    vk::Device dev;
    vk::PhysicalDevice physDev;
	vk::Instance instance;
    vk::CommandPool cmdPool;
    vk::Queue gfxQueue;

	size_t max_frames_in_flights;
};
} // namespace plt
