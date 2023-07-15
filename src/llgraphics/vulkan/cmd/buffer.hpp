#pragma once

#include <llgraphics/vulkan/utils.hpp>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace plt {

class VkCmdBuffer : public NoCopy {

    vk::CommandBuffer buffer;

public:
    VkCmdBuffer() = default;
    vk::CommandBuffer *operator->() {
        return &this->buffer;
    }

    vk::CommandBuffer &buf() {
        return this->buffer;
    }

    static Result<VkCmdBuffer> create(
        vk::Device device,
        vk::CommandPool pool,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) {
        VkCmdBuffer buffer;

        auto allocInfo = vk::CommandBufferAllocateInfo()
                             .setCommandPool(pool)
                             .setLevel(level)
                             .setCommandBufferCount(1);

        vkTry$(device.allocateCommandBuffers(&allocInfo, &buffer.buffer));

        return std::move(buffer);
    }

    Result<> start(vk::CommandBufferUsageFlags flags = {}) {

        auto beginInfo = vk::CommandBufferBeginInfo()
                             .setFlags(flags);

        vkTry$(this->buffer.begin(&beginInfo));

        return {};
    }

    Result<> end() {
        this->buffer.end();
        return {};
    }

    VkCmdBuffer(VkCmdBuffer &&) = default;
    VkCmdBuffer &operator=(VkCmdBuffer &&) = default;

    ~VkCmdBuffer() = default;
};

} // namespace plt
