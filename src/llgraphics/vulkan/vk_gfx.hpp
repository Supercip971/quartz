#pragma once

#include <vulkan/vulkan.hpp>

#include <vulkan/vulkan_handles.hpp>

#include "llgraphics/gfx.hpp"
#include "llgraphics/vulkan/cmd/buffer.hpp"
#include "llgraphics/vulkan/device/physical.hpp"
#include "llgraphics/vulkan/pipeline/shaders.hpp"
#include "llgraphics/vulkan/surface/swapchain.hpp"
#include "llgraphics/vulkan/utils.hpp"
#include "utils/traits.hpp"

namespace plt {

class VkGfx : public NoCopy, public Gfx {

public:
    Result<> setupVulkan();
    Result<> attachVulkan(Window *window);

    Result<> drawFrame();

    Result<> createInstance();

    Result<> setupDebugMessanger();

    Result<> setupSurface(Window *window);

    Result<> pickPhysicalDevice();

    Result<> setupLogicalDevice();

    Result<> setupSwapchain(Window *window);

    Result<> createImageViews();

    Result<> createShaderPipeline();

    Result<> createRenderPass();

    Result<> createGraphicPipeline();

    Result<> createFramebuffers();

    Result<> createCommandPool();

    Result<> createCommandBuffers();

    Result<> createSyncObjects();
    Result<> recordRenderCommands(uint32_t imageIndex);

    QueueFamilyIndices findPhysicalDeviceQueueFamily();

    void vulkanDeinit() {

        info$("deinitilizing vulkan");
        this->LogicalDevice.waitIdle();

        for (auto it = this->deinit_funcs.rbegin(); it != this->deinit_funcs.rend(); ++it) {
            (*it)(this);
        }

        this->deinit_funcs.clear();

        info$("vulkan deinitilized");
    }

    QueueFamilyIndices findPhysicalDeviceQueueFamily(vk::PhysicalDevice device);

    bool isDeviceSuitable(vk::PhysicalDevice device);

    bool hasDeviceExtensions(vk::PhysicalDevice device);

    SwapchainSupportInfos querySwapchainSupport(vk::PhysicalDevice device);

private:
    std::vector<std::function<void(VkGfx *)>> deinit_funcs;

    /* ---- instance ---- */
    vk::Instance instance;
    vk::DebugUtilsMessengerEXT debugMessenger;

    /* ---- surface ---- */
    vk::SurfaceKHR surface;

    /* ---- device ----*/
    vk::PhysicalDevice physicalDevice;
    vk::Device LogicalDevice;

    QueueFamilyIndices queueFamilyIndices;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    /* ---- swapchains ---- */
    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapchainImages;

    std::vector<vk::ImageView> swapchainImageViews;

    vk::Format swapchainImageFormat;
    vk::Extent2D swapchainExtent;

    /* ---- graphic pipeline ---- */
    Shader vertShader;
    Shader fragShader;

    ShaderPipeline shaderPipeline;

    // render pass - framebuffer attachment
    vk::RenderPass renderPass;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicPipeline;

    /* ---- framebuffers ---- */

    std::vector<vk::Framebuffer> swapchainFramebuffers;

    /* ---- commands---- */

    vk::CommandPool commandPool;

    VkCmdBuffer cmdBuffer;

    /* ---- synchronization ---- */

    // signal that the image has been acquired and is ready for rendering
    vk::Semaphore imageAvailableSemaphore;

    // signal that rendering has finished and presentation can happen
    vk::Semaphore renderFinishedSemaphore;

    // signal that a frame has finished rendering
    vk::Fence inFlightFence;

    /* ---- misc ---- */
    static constexpr std::array deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
} // namespace plt
