#pragma once
#include <cstdint>
#include <llgraphics/vulkan/mem/mem.hpp>
#include <llgraphics/vulkan/mesh/vertex.hpp>
#include <llgraphics/vulkan/utils.hpp>

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "llgraphics/vulkan/cmd/buffer.hpp"
#include "llgraphics/vulkan/mem/allocator.hpp"
#include "llgraphics/vulkan/pipeline/uniforms.hpp"

namespace plt {

template <VertexType T = NVertex>
class VkMesh {

    T *_vertices;

    uint32_t *_indices;

    size_t _numVertices;
    size_t _numIndices;

    VertexDescription _description = T::description();

    GpuMemory _vertexBuffer;
    GpuMemory _indexBuffer;
	GpuMemory _uniforms;

	size_t _uniform_size;
	void* _mapped_uniform;

public:
    VkMesh() {
        _vertices = nullptr;
        _indices = nullptr;
        _description = T::description();
    }

    VkMesh(T *vertices, uint32_t *indices, size_t numVertices, size_t numIndices, size_t uniform_size) {
        this->_vertices = vertices;
        this->_indices = indices;
        this->_numVertices = numVertices;
        this->_numIndices = numIndices;
		this->_uniform_size = uniform_size;
        _description = T::description();
    }

    VkMesh(T *vertices, size_t numVertices) {
        this->_vertices = vertices;
        this->_indices = nullptr;
        this->_numVertices = numVertices;
        this->_numIndices = 0;
        _description = T::description();
    }

	template<typename N>
	N* mapped_uniform() {
		return _mapped_uniform;
	}

	template<typename N>
	void mapped_uniform(N& val) {
		*((N*)_mapped_uniform) = val;
	}

    Result<> allocateGpuBuffers(GpuCtx dev) {

		warn$("use the same buffer for staging, index and vertex buffer");


		try$(MemoryManager::the().grouped()
			.allocate(BUF_USAGE_TRANSFER_DST | BUF_USAGE_VERTEX_BUFFER,(size_t)(_numVertices * sizeof(T)), vk::MemoryPropertyFlagBits::eDeviceLocal, &_vertexBuffer)
			.allocate(BUF_USAGE_TRANSFER_DST | BUF_USAGE_INDEX_BUFFER,(size_t)(_numIndices * sizeof(uint32_t)), vk::MemoryPropertyFlagBits::eDeviceLocal, &_indexBuffer)
			.allocate(BUF_USAGE_UNIFORM_BUFFER | BUF_USAGE_DYNAMIC | BUF_USAGE_CPU_USABLE, _uniform_size, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &_uniforms)
			.execute());

		(void)(dev);


		info$("a");
        try$(_vertexBuffer.stagedCopy(_vertices, _numVertices * sizeof(T)));

        if (_indices != nullptr) {

		info$("b");

            try$(_indexBuffer.stagedCopy(_indices, _numIndices * sizeof(uint32_t)));
        }
		info$("c");


		try$(_uniforms.map_frame());

        return {};
    }


	void *getUniform()
	{

		return _uniforms.get_current_frame_mapping().unwrap();
	}

    void releaseGpuBuffers() {

		MemoryManager::the().release(_vertexBuffer);

		MemoryManager::the().release(_indexBuffer);

		_uniforms.unmap_frame();
		MemoryManager::the().release(_uniforms);

    }

    auto description() const {
        return _description;
    }

    auto VertexBuffer() {
        return _vertexBuffer.buffer().unwrap();
    }

    auto verticesCount() {
        return _numVertices;
    }

    auto indexBuffer() {
        return _indexBuffer.buffer().unwrap();
    }

    auto indicesCount() {
        return _numIndices;
    }
};

} // namespace plt
