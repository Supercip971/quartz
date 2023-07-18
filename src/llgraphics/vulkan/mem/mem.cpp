#include "mem.hpp"
#include <cstdint>

#include "llgraphics/vulkan/vk_gfx.hpp"

#include "llgraphics/vulkan/utils.hpp"

namespace plt {

Result<uint32_t> GpuMemory::findMemoryType(GpuCtx ctx, uint32_t typeFilter, vk::MemoryPropertyFlags props) {

    auto memProperties = ctx.physDev.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & props) == props)) {
            return i;
        }
    }

    return Result<uint32_t>::err("failed to find suitable memory type: {x} {x}", (uint32_t)typeFilter, (uint32_t)props);
}

Result<GpuMemory> GpuMemory::allocate(GpuCtx ctx, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {

    debug$("allocating gpu memory: {}", size);

    vk::Buffer buffer;
    buffer = ctx.dev.createBuffer(vk::BufferCreateInfo()
                                      .setSize(size)
                                      .setUsage(usage)
                                      .setSharingMode(vk::SharingMode::eExclusive));

    auto memRequirements = ctx.dev.getBufferMemoryRequirements(buffer);

    auto memory = ctx.dev.allocateMemory(vk::MemoryAllocateInfo()
                                             .setAllocationSize(memRequirements.size)
                                             .setMemoryTypeIndex(try$(findMemoryType(ctx, memRequirements.memoryTypeBits, properties))));

    ctx.dev.bindBufferMemory(buffer, memory, 0);

    return GpuMemory(ctx, memory, buffer, size);
}

} // namespace plt
