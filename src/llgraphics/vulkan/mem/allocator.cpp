#include "llgraphics/vulkan/mem/allocator.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include "llgraphics/vulkan/utils.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
namespace plt 
{


	static MemoryManager mem_mgr = MemoryManager();
	

		
		Result<AllocatedResult*> MemoryManager::get_current_frame_mem(AllocatedMemId id)
		{

			if(_allocated_mems.count(id) == 0)
			{
				error$("MemoryManager::get_current_frame_mem: id not found: {}", id);

				for(auto& it : _allocated_mems)
				{
					error$("MemoryManager::get_current_frame_mem: id: {}", it.first);
				}

				return Result<AllocatedResult*>::err("MemoryManager::get_current_frame_mem: id not found: {}", id);
			}

			if(_allocated_mems.find(id)->second._mems.size() > 1)
			{
				return &_allocated_mems.find(id)->second._mems[_in_flight_frame_id];
			}
			return &_allocated_mems.find(id)->second._mems[0];
		}

		Result<> MemoryManager::map(GpuMemory& mem, bool single_frame)
		{

			if(single_frame)
			{

				auto it = try$(get_current_frame_mem(mem.id()));

				vkTry$(vmaMapMemory(_allocator, it->_allocation, &it->mapping));
			}
			else 
			{
				auto v = &_allocated_mems.find(mem.id())->second;
				for(auto& it : v->_mems)
				{
					vkTry$(vmaMapMemory(_allocator, it._allocation, &it.mapping));
				}
			}

			return {};
		}
		void MemoryManager::unmap(GpuMemory& mem, bool single_frame)
		{
			if(single_frame)
			{
				auto it = (get_current_frame_mem(mem.id())).unwrap();
			
				vmaUnmapMemory(_allocator, it->_allocation);
				it->mapping = nullptr;
			}
			else 
			{
				for(auto& it : _allocated_mems.find(mem.id())->second._mems)
				{
					vmaUnmapMemory(_allocator, it._allocation);
					it.mapping = nullptr;
				}
			}
		}
		
	Result<void*> MemoryManager::get_current_frame_mapping(GpuMemory& mem)
	{
		
		auto it = try$(get_current_frame_mem(mem.id()));
		if(it->mapping == nullptr)
		{
			return Result<void*>::err("MemoryManager::get_current_frame_mapping: mapping is null");
		}

		return it->mapping;
	}

	Result<> MemoryManager::initialize(GpuCtx ctx)
	{
		info$("Initializing MemoryManager");
		_ctx = ctx;


		VmaAllocatorCreateInfo create_info = {};
		create_info.device = _ctx.dev;
		create_info.physicalDevice = _ctx.physDev;
		create_info.instance = _ctx.instance;

		this->_allocated_mems = std::unordered_map<AllocatedMemId, FrameAllocatedMems>();
		vkTry$(vmaCreateAllocator(&create_info, &_allocator));

		return {};
	}
	MemoryManager& MemoryManager::the() 
	{
		return mem_mgr;
	}

	Allocator MemoryManager::grouped()
	{

		return Allocator();
	}


	constexpr vk::Flags<vk::BufferUsageFlagBits> castedUsage(BufferUsage usage)
	{
		vk::Flags<vk::BufferUsageFlagBits> result ;
		if (usage & BUF_USAGE_TRANSFER_SRC)
			result = result | vk::BufferUsageFlagBits::eTransferSrc;
		if (usage & BUF_USAGE_TRANSFER_DST)
			result = result | vk::BufferUsageFlagBits::eTransferDst;
		if (usage & BUF_USAGE_UNIFORM_BUFFER)
			result = result | vk::BufferUsageFlagBits::eUniformBuffer;
		if (usage & BUF_USAGE_STORAGE_BUFFER)
			result = result | vk::BufferUsageFlagBits::eStorageBuffer;
		if (usage & BUF_USAGE_INDEX_BUFFER)
			result = result | vk::BufferUsageFlagBits::eIndexBuffer;
		if (usage & BUF_USAGE_VERTEX_BUFFER)
			result = result | vk::BufferUsageFlagBits::eVertexBuffer;
		if (usage & BUF_USAGE_INDIRECT_BUFFER)
			result = result | vk::BufferUsageFlagBits::eIndirectBuffer;

		return result;
	}

	constexpr VmaMemoryUsage castedVmaUsage(BufferUsage usage)
	{
		VmaMemoryUsage result = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		if (usage & BUF_USAGE_CPU_USABLE)
			result = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		if (usage & BUF_USAGE_DYNAMIC)
			result = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

		return result;

	}

	void MemoryManager::uninitialize()
	{
		info$("Uninitializing MemoryManager");
		vmaDestroyAllocator(_allocator);
	}

	void MemoryManager::release(GpuMemory& mem)
	{

		auto it = _allocated_mems.find(mem.id());

		for (auto& mem : it->second._mems)
		{
			vmaDestroyBuffer(_allocator, mem._buffer, mem._allocation);
		}

		it->second._mems.clear();

		_allocated_mems.erase(it);
	}
	Result<> MemoryManager::allocate(AllocatedGroup& IO_group)
	{
		
		for (auto& ctx : IO_group)
		{
			size_t count = 1;
			if(ctx.usage & BUF_USAGE_DYNAMIC)
				count = this->_in_flight_frames;

			// random ID
			AllocatedMemId id = (AllocatedMemId)rand();

			info$("allocating id: {} (subframes: {})", id, count);
			this->_allocated_mems[id] = FrameAllocatedMems();
			for(size_t i = 0; i < count; i++)
			{


				vk::BufferCreateInfo buffer_info = {};
				buffer_info.size = ctx.size;
				buffer_info.usage = castedUsage(ctx.usage);

				VmaAllocationCreateInfo alloc_info = {};
				alloc_info.usage = castedVmaUsage(ctx.usage);
				alloc_info.flags = ctx.usage & BUF_USAGE_CPU_USABLE ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : 0;
				alloc_info.requiredFlags = ctx.mem_flags;
	
				info$("id: {} has usage: {:#0b}", id, (uint32_t)ctx.usage);
				VkBuffer buffer;
				VmaAllocation allocation;

				VkBufferCreateInfo nbuffer_info = buffer_info;
				vkTry$(vmaCreateBuffer(_allocator, &nbuffer_info, &alloc_info, &buffer, &allocation, nullptr));
				
				this->_allocated_mems[id]._mems.push_back({ ctx, buffer, allocation, nullptr});

				if(ctx.result)
					*ctx.result = GpuMemory(id);
			}

		}

		return {};
	}


	Result<VkBuffer> MemoryManager::buffer(const GpuMemory& mem) 
	{

		auto it = try$(get_current_frame_mem(mem.id()));
		return {it->_buffer};
	}
	Result<> MemoryManager::stagedCopy(GpuMemory& mem, void* data, size_t len)
	{
		auto it = try$(get_current_frame_mem(mem.id()));

        if (len > it->_creation_ctx.size) {
            return Result<>::err("Data size: {} is larger than buffer size: {}", len, it->_creation_ctx.size);
        }
		
		GpuMemory stagingBuffer{0}; 
		try$(MemoryManager::grouped()
				.allocate(BUF_USAGE_CPU_USABLE | BUF_USAGE_TRANSFER_SRC, len, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &stagingBuffer)
				.execute());

		try$(stagingBuffer.map());
		memcpy(try$(stagingBuffer.get_current_frame_mapping()), data, len);
		stagingBuffer.unmap();

        auto cmdBuffer = try$(plt::VkCmdBuffer::create(_ctx.dev, _ctx.cmdPool));

        cmdBuffer.start(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        {
            cmdBuffer->copyBuffer(try$(stagingBuffer.buffer()), try$(mem.buffer()), vk::BufferCopy(0, 0, len));
        }
        cmdBuffer.end();
        _ctx.gfxQueue.submit(
            vk::SubmitInfo()
                .setCommandBufferCount(1)
                .setPCommandBuffers(&cmdBuffer.buf()),
            vk::Fence());
        _ctx.gfxQueue.waitIdle();

        cmdBuffer.release();

		MemoryManager::release(stagingBuffer);

        return {};
   	}


}
