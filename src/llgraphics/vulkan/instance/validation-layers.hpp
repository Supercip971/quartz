#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace plt {

#ifdef NRELEASE
[[maybe_unused]] static constexpr bool enableValidationLayers = false;
#else
[[maybe_unused]] static constexpr bool enableValidationLayers = true;
#endif

bool checkDriverValidationLayers();

std::vector<char const *> getValidationLayers();

vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreationInfo();

} // namespace plt
