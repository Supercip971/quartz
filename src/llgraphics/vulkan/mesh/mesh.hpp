#pragma once 
#include <llgraphics/vulkan/utils.hpp>

#include <llgraphics/vulkan/mesh/vertex.hpp>
#include <llgraphics/vulkan/mem/mem.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace plt 
{
	

	template<VertexType T=NVertex>
	class VkMesh 
	{

		T *_vertices;

		uint32_t *_indices;

		size_t _numVertices;
		size_t _numIndices;

		VertexDescription _description = T::description();

		GpuMemory _vertexBuffer;
		
		public:

		VkMesh()
		{
			_vertices = nullptr;
			_indices = nullptr;
			_description = T::description();
		}

		VkMesh(T *vertices, uint32_t *indices, size_t numVertices, size_t numIndices)
		{
			this->_vertices = vertices;
			this->_indices = indices;
			this->_numVertices = numVertices;
			this->_numIndices = numIndices;
			_description = T::description();
		}

		VkMesh(T *vertices, size_t numVertices)
		{
			this->_vertices = vertices;
			this->_indices = nullptr;
			this->_numVertices = numVertices;
			this->_numIndices = 0;
			_description = T::description();
		}

		Result<> allocateGpuBuffers(GpuCtx dev)
		{
			_vertexBuffer = try$(GpuMemory::allocate(dev, _numVertices * sizeof(T), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

			volatile auto mapped = (volatile T*)_vertexBuffer.map();

			memcpy((void*)mapped, _vertices, _numVertices * sizeof(T));

			_vertexBuffer.unmap();
			return {};
		}

		void releaseGpuBuffers()
		{

			_vertexBuffer.deallocate();
		}

		auto description() const
		{
			return _description;
		}

		auto VertexBuffer()
		{
			return _vertexBuffer.buffer();
		}

		auto verticesCount()
		{
			return _numVertices;
		}


		


	};

}
