#pragma once

#include <vulkan/vulkan.hpp>

#include "utils/result.hpp"

namespace plt {

[[maybe_unused]] static inline Result<> vkTry(vk::Result res) {
    if (res != vk::Result::eSuccess) {
        return Result<>::err("vulkan error: {}", (uint32_t)res);
    }
    return {};
};

#define vkTry$(r) try$(vkTry(r))

	struct GpuCtx 
	{
		vk::Device dev;
		vk::PhysicalDevice physDev;
	};
} // namespace plt
