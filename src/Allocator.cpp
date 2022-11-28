#include"Allocator.h"

namespace MUZI {
#ifdef __MUZI_ALLOCATOR_MOD_POOL__
	size_t MAllocator::object_num = 0;
	size_t MAllocator::pool_mem_total = 0;
	union MAllocator::MAllocatorRep* MAllocator::pool_mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__] = { nullptr };
	union MAllocator::MAllocatorRep* MAllocator::pool_start_free_pool_ptr = nullptr;
	union MAllocator::MAllocatorRep* MAllocator::pool_end_free_pool_ptr = nullptr;
	MAllocator::clearMemoryFunction MAllocator::cmf[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	void* MAllocator::cmf_arg = nullptr;

	inline size_t MAllocator::pool_RoundUp()
	{
		return 
			(__MUZI_ALLOCAOTR_MOD_POOL_ROUNDUP__(MAllocator::pool_mem_total) + __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1) 
			& -(__MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1);
	}
	inline size_t MAllocator::pool_freelist_index(size_t bytes)
	{
		return (bytes + __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1) / __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1;
	}

	void* MAllocator::pool_allocate(size_t type_size)
	{
		MAllocatorRep* ret_mem_ptr = nullptr;
		size_t pool_index = MAllocator::pool_freelist_index(type_size); // 由大小（八的倍数）获取其坐标
		size_t pool_index_specification = (pool_index + 1) * __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__; // 获取当前位置对应的内存申请规格

		constexpr size_t alloc_len = __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__;

		// 发现该点之前未分配或者该点之前分配的分配完了 
		if (MAllocator::pool_mem_pool[pool_index] == nullptr)
		{
			// 并且战备池已经空了
			if (MAllocator::pool_start_free_pool_ptr == MAllocator::pool_end_free_pool_ptr)
			{
				// 一半用以正常申请内容 一半用以战备池
				MAllocator::pool_mem_pool[pool_index] =
					static_cast<MAllocatorRep*>
					(::operator new(alloc_len * type_size * 2));

				// 获取战备池
				MAllocator::pool_start_free_pool_ptr = MAllocator::pool_mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MAllocator::pool_end_free_pool_ptr = MAllocator::pool_start_free_pool_ptr + alloc_len * type_size;

				// 记录申请总量
				MAllocator::pool_mem_total += alloc_len;

				// 将申请到的内存进行切割(从尾部开始往上切割)
				MAllocatorRep* list_next_ptr =
					(MAllocatorRep*)((char*)MAllocator::pool_start_free_pool_ptr - pool_index_specification);
				list_next_ptr->next = nullptr;
				MAllocatorRep* list_next_ptr_temp = list_next_ptr;

				// 构建空闲单向链表来切割内存
				for (int i = 0; i < __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__; ++i)
				{
					list_next_ptr =
						(MAllocatorRep*)((char*)list_next_ptr - pool_index_specification);
					list_next_ptr->next = list_next_ptr_temp;
					list_next_ptr_temp = list_next_ptr;
				}

				ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
				MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_mem_pool[pool_index]->next;
			}
			else// 战备池有剩余
			{
				size_t pool_size = MAllocator::pool_end_free_pool_ptr - MAllocator::pool_start_free_pool_ptr;
				if (type_size > pool_size)
				{
					size_t pool_block_count = pool_size / type_size;
				}
			}
		}
		else
		{
			// 如果已经分配过且仍然有空余
			MAllocatorRep* list_next_ptr = MAllocator::pool_mem_pool[pool_index]->next;
			ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
			MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_mem_pool[pool_index]->next;

		}

		return static_cast<void*>(ret_mem_ptr);
	}
	void MAllocator::pool_deallocate(void* ptr, size_t mem_size)
	{
		char* p = nullptr;

	}
	void* MAllocator::pool_reallocate()
	{
		return nullptr;
	}
	void* MAllocator::pool_allocate_array(size_t type_array_size)
	{
		char* p = nullptr;

		return p;
	}
	void MAllocator::pool_deallocate_array(void* ptr, size_t mem_size)
	{
		char* p = nullptr;

	}
#endif // __MUZI_ALLOCATOR_MOD_POOL__


};
