#pragma once
#include <llgraphics/vulkan/utils.hpp>

#include <vulkan/vulkan_structs.hpp>

namespace plt {

class GpuMemory {

    vk::DeviceMemory _memory;
    vk::Buffer _buffer;

    GpuCtx _owner;
    size_t _size;

public:
    GpuMemory(GpuCtx owner, vk::DeviceMemory memory, vk::Buffer buffer, size_t size) {
        _owner = owner;
        _memory = memory;
        _buffer = buffer;
        _size = size;
    }

    GpuMemory() {
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

    void *map() {
        return _owner.dev.mapMemory(_memory, 0, _size);
    }

    void unmap() {
        _owner.dev.unmapMemory(_memory);
    }

    auto deallocate() {
        _owner.dev.destroyBuffer(_buffer);
        _owner.dev.freeMemory(_memory);

        _memory = nullptr;
    };

    static Result<GpuMemory> allocate(GpuCtx ctx, size_t size, vk::BufferUsageFlags usage = {}, vk::MemoryPropertyFlags properties = {});

    static Result<uint32_t> findMemoryType(GpuCtx ctx, uint32_t typeFilter, vk::MemoryPropertyFlags props);
};

} // namespace plt
