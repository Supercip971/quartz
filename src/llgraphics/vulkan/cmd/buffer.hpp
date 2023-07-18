#pragma once

#include <llgraphics/vulkan/utils.hpp>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace plt {

class VkCmdBuffer : public NoCopy {

    vk::CommandBuffer buffer;
    vk::Device dev;
    vk::CommandPool pool;

public:
    VkCmdBuffer() = default;
    VkCmdBuffer(vk::CommandBuffer buffer, vk::Device dev, vk::CommandPool pool) : buffer(buffer), dev(dev), pool(pool) {}

    vk::CommandBuffer *operator->() {
        return &this->buffer;
    }

    vk::CommandBuffer &buf() {
        return this->buffer;
    }

    void release() {
        dev.freeCommandBuffers(pool, buffer);
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

        buffer.dev = device;
        buffer.pool = pool;

        return std::move(buffer);
    }

    static Result<std::vector<VkCmdBuffer>> create_buffers(
        vk::Device device,
        vk::CommandPool pool,
        size_t count,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) {

        std::vector<vk::CommandBuffer> buffers(count);
        auto allocInfo = vk::CommandBufferAllocateInfo()
                             .setCommandPool(pool)
                             .setLevel(level)
                             .setCommandBufferCount(count);

        vkTry$(device.allocateCommandBuffers(&allocInfo, buffers.data()));

        std::vector<VkCmdBuffer> cmd_buffers;
        cmd_buffers.reserve(count);
        for (auto &buffer : buffers) {
            cmd_buffers.push_back(VkCmdBuffer(buffer, device, pool));
        }

        return std::move(cmd_buffers);
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
