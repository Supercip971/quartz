#pragma once
#include <llgraphics/vulkan/mem/mem.hpp>
#include <llgraphics/vulkan/mesh/vertex.hpp>
#include <llgraphics/vulkan/utils.hpp>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "llgraphics/vulkan/cmd/buffer.hpp"

namespace plt {

template <VertexType T = NVertex>
class VkMesh {

    T *_vertices;

    uint32_t *_indices;

    size_t _numVertices;
    size_t _numIndices;

    VertexDescription _description = T::description();

    GpuMemory _vertexBuffer;

public:
    VkMesh() {
        _vertices = nullptr;
        _indices = nullptr;
        _description = T::description();
    }

    VkMesh(T *vertices, uint32_t *indices, size_t numVertices, size_t numIndices) {
        this->_vertices = vertices;
        this->_indices = indices;
        this->_numVertices = numVertices;
        this->_numIndices = numIndices;
        _description = T::description();
    }

    VkMesh(T *vertices, size_t numVertices) {
        this->_vertices = vertices;
        this->_indices = nullptr;
        this->_numVertices = numVertices;
        this->_numIndices = 0;
        _description = T::description();
    }

    Result<> allocateGpuBuffers(GpuCtx dev) {
        auto stagingBuffer = try$(GpuMemory::allocate(dev, _numVertices * sizeof(T), vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        _vertexBuffer = try$(GpuMemory::allocate(dev, _numVertices * sizeof(T), vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal));

        auto volatile mapped = (T volatile *)stagingBuffer.map();
        memcpy((void *)mapped, _vertices, _numVertices * sizeof(T));
        stagingBuffer.unmap();

        auto cmdBuffer = try$(VkCmdBuffer::create(dev.dev, dev.cmdPool));

        cmdBuffer.start(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        {
            cmdBuffer->copyBuffer(stagingBuffer.buffer(), _vertexBuffer.buffer(), vk::BufferCopy(0, 0, _numVertices * sizeof(T)));
        }
        cmdBuffer.end();

        dev.gfxQueue.submit(
            vk::SubmitInfo()
                .setCommandBufferCount(1)
                .setPCommandBuffers(&cmdBuffer.buf()),
            vk::Fence());
        dev.gfxQueue.waitIdle();

        cmdBuffer.release();

        stagingBuffer.deallocate();

        return {};
    }

    void releaseGpuBuffers() {

        _vertexBuffer.deallocate();
    }

    auto description() const {
        return _description;
    }

    auto VertexBuffer() {
        return _vertexBuffer.buffer();
    }

    auto verticesCount() {
        return _numVertices;
    }
};

} // namespace plt
