#pragma once
#include <llgraphics/vulkan/cmd/buffer.hpp>
#include <llgraphics/vulkan/utils.hpp>

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_structs.hpp>

#include "llgraphics/vulkan/cmd/buffer.hpp"

namespace plt {
/*
class GpuMemory {

    vk::DeviceMemory _memory = nullptr;
    vk::Buffer _buffer;

	VkDeviceSize _offset = 0;

    GpuCtx _owner;
    size_t _size;

    static Result<GpuMemory> allocate(GpuCtx ctx, size_t size, vk::BufferUsageFlags usage = {}, vk::MemoryPropertyFlags properties = {});

    static Result<uint32_t> findMemoryType(GpuCtx ctx, uint32_t typeFilter, vk::MemoryPropertyFlags props);


public:
    GpuMemory(GpuCtx owner, vk::DeviceMemory memory, vk::Buffer buffer, size_t size, VkDeviceSize offset = 0) {
        _owner = owner;
        _memory = memory;
        _buffer = buffer;
        _size = size;
		_offset = offset;
    }

    GpuMemory() {
        _memory = nullptr;
    }

    ~GpuMemory() {
    }

    auto buffer() const {
        return _buffer;
    }

    auto memory() const {
        return _memory;
    }

    auto owner() const {
        return _owner;
    }

    auto size() const {
        return _size;
    }

	auto offset() const 
	{
		return _offset;
	}

	
    void *map() {
     //   return _owner.dev.mapMemory(_memory, _offset, _size);
    }

    void unmap() {
    //    _owner.dev.unmapMemory(_memory);
    }

    bool valid() const {
        return _memory != nullptr;
    }
    auto deallocate() {
        _owner.dev.destroyBuffer(_buffer);
        _owner.dev.freeMemory(_memory);

        _memory = nullptr;
    };

    Result<> stagedCopy(void *data, size_t size) {

        if (size > _size) {
            return Result<>::err("Data size: {} is larger than buffer size: {}", size, _size);
        }
        auto stagingBuffer = try$(GpuMemory::allocate(
            _owner,
            size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        auto volatile mapped = (void *)stagingBuffer.map();
        memcpy((void *)mapped, data, size);
        stagingBuffer.unmap();

        auto cmdBuffer = try$(plt::VkCmdBuffer::create(_owner.dev, _owner.cmdPool));

        cmdBuffer.start(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        {
            cmdBuffer->copyBuffer(stagingBuffer.buffer(), buffer(), vk::BufferCopy(0, 0, size));
        }
        cmdBuffer.end();

        _owner.gfxQueue.submit(
            vk::SubmitInfo()
                .setCommandBufferCount(1)
                .setPCommandBuffers(&cmdBuffer.buf()),
            vk::Fence());
        _owner.gfxQueue.waitIdle();

        cmdBuffer.release();

        stagingBuffer.deallocate();

        return {};
    }
};*/

} // namespace plt
