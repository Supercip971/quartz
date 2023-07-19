#pragma once 

#include <stdint.h>
#include <llgraphics/vulkan/utils.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include "llgraphics/vulkan/mem/mem.hpp"
#include <vk_mem_alloc.h>

/* Why does this exist ?  
 * The problem is that buffers states are more complicated than just a pointer. 
 * I wanted to handle the case where the buffer is used each frame.
 * And because of the fact that we support in-flight frame rendering, we need to
 * store multiple buffers states.
 * So a mesh would need multiple buffers and would need to know which 
 * frame we are to edit a buffer. And would make spaghetti code.
 * So I need to abstract the access and allocation of buffers.
 */
namespace plt 
{
	using AllocatedMemId = uint32_t;
	
	enum BufferUsageBits : uint32_t 
	{

		BUF_USAGE_VERTEX_BUFFER = (1 << 1),
		BUF_USAGE_INDEX_BUFFER = (1 << 2),
		BUF_USAGE_UNIFORM_BUFFER = (1 << 3),
		BUF_USAGE_STORAGE_BUFFER = (1 << 4),
		BUF_USAGE_TRANSFER_SRC = (1 << 5),
		BUF_USAGE_TRANSFER_DST = (1 << 6),
		BUF_USAGE_INDIRECT_BUFFER = (1 << 7),
		BUF_USAGE_CPU_USABLE = (1 << 8),
		BUF_USAGE_DYNAMIC = (1 << 9),
	};

	using BufferUsage = uint32_t;

	class GpuMemory;

	struct AllocatedCtx 
	{
			BufferUsage usage;
			VkDeviceSize size;
			VkMemoryPropertyFlags mem_flags;
			GpuMemory* result;
	};

	struct AllocatedResult 
	{
		AllocatedCtx _creation_ctx;
		vk::Buffer _buffer;
		VmaAllocation _allocation;
		void* mapping;
	};

	using AllocatedGroup = std::vector<AllocatedCtx>; 
	class Allocator;
	class MemoryManager 
	{

		GpuCtx _ctx;
		size_t _in_flight_frames;
		size_t _in_flight_frame_id;

		struct FrameAllocatedMems 
		{
			GpuCtx _ctx;
			std::vector<AllocatedResult> _mems;

			FrameAllocatedMems(): _ctx(), _mems() {}
		};
		
		Result<AllocatedResult*> get_current_frame_mem(AllocatedMemId id);



		// if this is too slow, we may changes how we store the data 

		std::unordered_map<AllocatedMemId, FrameAllocatedMems> _allocated_mems;


		VmaAllocator _allocator;
		friend class Allocator;
		Result<> allocate(AllocatedGroup& IO_group);
		public: 

		MemoryManager() {}

		MemoryManager(GpuCtx ctx): _ctx(ctx) {}

		static MemoryManager& the();

		Result<> initialize(GpuCtx ctx);

		void start_new_frame(uint32_t frame_id);
		void set_in_flight_frames(uint32_t in_flight_frames){ _in_flight_frames = in_flight_frames; }

		void release(GpuMemory& mem);


		Result<> map(GpuMemory& mem, bool single_frame);
		void unmap(GpuMemory& mem, bool single_frame);

		Result<> stagedCopy(GpuMemory& mem, void* data, size_t len);

		Result<VkBuffer> buffer(const GpuMemory& mem);
		Result<void*> get_current_frame_mapping(GpuMemory& mem);

		Allocator grouped();
	};

	class GpuMemory 
	{
		AllocatedMemId _id;

		public: 
		GpuMemory(){};

		GpuMemory(AllocatedMemId id) : _id(id) {}
		AllocatedMemId id() const { return _id; }

		Result<> map_frame(){
			try$(MemoryManager::the().map(*this, true));
			return {};
		}

		void unmap_frame(){
			MemoryManager::the().unmap(*this, true);
		}

		Result<> map(){
			try$(MemoryManager::the().map(*this, false));
			return {};
		}

		void unmap(){
			(MemoryManager::the().unmap(*this, false));
		}

		Result<void*> get_current_frame_mapping()
		{
			return try$(MemoryManager::the().get_current_frame_mapping(*this));
		}

		Result<> stagedCopy(void* data, size_t len)
		{
			return MemoryManager::the().stagedCopy(*this, data, len);
		}
		Result<vk::Buffer> buffer() const
		{
			return {try$(MemoryManager::the().buffer(*this))};
		}

	};
	class Allocator 
	{

		AllocatedGroup  _stacked_allocated_ctxs;
		MemoryManager& _manager = MemoryManager::the();
		friend class MemoryManager;
		Allocator() {}


		Allocator(const Allocator& other, AllocatedCtx ctx)
		{
			_stacked_allocated_ctxs = other._stacked_allocated_ctxs;
			_stacked_allocated_ctxs.push_back(ctx);
		}

		public:

		Allocator allocate(
				BufferUsage usage, 
				VkDeviceSize size, 
				vk::MemoryPropertyFlags mem_flags,
				GpuMemory* result) const 
		{
			if(size != 0)
			{
				auto v = AllocatedCtx{usage, size, (VkMemoryPropertyFlags) mem_flags, result};

				return Allocator(*this, v);
	
			}

			return Allocator(*this);
		}

		Allocator allocate(
				BufferUsage usage, 
				VkDeviceSize size, 
				VkMemoryPropertyFlags mem_flags) const 
		{
			auto v = AllocatedCtx{usage, size, (VkMemoryPropertyFlags) mem_flags, nullptr};

			return Allocator(*this, v);
		}


		Result<AllocatedGroup> execute()
		{
			try$(_manager.allocate(_stacked_allocated_ctxs));	
			return _stacked_allocated_ctxs;
		}

	};


}
