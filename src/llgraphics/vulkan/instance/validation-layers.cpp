
#include <vector>
#include <vulkan/vulkan.hpp>

#include "llgraphics/vulkan/vk_gfx.hpp"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "utils/log.hpp"
#include "validation-layers.hpp"

namespace plt {
const std::vector<char const *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

std::vector<char const *> getValidationLayers() {
    return validationLayers;
}

bool checkDriverValidationLayers() {
    auto layers = vk::enumerateInstanceLayerProperties();
    for (auto &layer : validationLayers) {
        bool found = false;
        for (auto &l : layers) {
            if (strcmp(layer, l.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

static std::string validationLayerFlagStr(VkDebugUtilsMessageTypeFlagsEXT flags) {
    std::string str = "";
    if (flags & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        str += "general ";
    }
    if (flags & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        str += "validation ";
    }
    if (flags & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        str += "performance ";
    }
    if (flags & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
        str += "device address binding";
    }

    return str;
}
static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
    void *pUserData) {
    (void)pUserData;

    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        error$("[VULKAN]({}): {}", validationLayerFlagStr(messageType), pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        warn$("[VULKAN]({}): {}", validationLayerFlagStr(messageType), pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        info$("[VULKAN]({}): {}", validationLayerFlagStr(messageType), pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        debug$("[VULKAN]({}): {}", validationLayerFlagStr(messageType), pCallbackData->pMessage);
    }

    return VK_FALSE;
}
vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreationInfo() {

    return vk::DebugUtilsMessengerCreateInfoEXT(
        {},
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        validationLayerCallback,
        nullptr);
}

extern "C" VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo, VkAllocationCallbacks const *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
extern "C" void vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks const *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

Result<> VkGfx::setupDebugMessanger() {

    if (!enableValidationLayers) {
        return {};
    }

    debug$("setting up debug messenger");

    auto createInfo = debugMessengerCreationInfo();

    vkTry$(instance.createDebugUtilsMessengerEXT(&createInfo, nullptr, &debugMessenger));

    this->deinit_funcs.push_back([](VkGfx *gfx) {
        debug$("destroying debug messenger");
        gfx->instance.destroyDebugUtilsMessengerEXT(gfx->debugMessenger);
    });

    return {};
}

} // namespace plt
