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
        _vertexBuffer = try$(GpuMemory::allocate(dev, _numVertices * sizeof(T), vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal));

        try$(_vertexBuffer.stagedCopy(_vertices, _numVertices * sizeof(T)));

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
