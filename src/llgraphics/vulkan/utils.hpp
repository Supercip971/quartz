#pragma once 


#include "utils/result.hpp"
#include <vulkan/vulkan.hpp>

namespace plt 
{


[[maybe_unused]] static inline Result<> vkTry(vk::Result res) {
    if (res != vk::Result::eSuccess) {
        return Result<>::err("vulkan error: {}", (uint32_t)res);
    }
    return {};
};

#define vkTry$(r) try$(vkTry(r))


}

