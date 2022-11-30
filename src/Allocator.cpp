#include"Allocator.h"

namespace MUZI {

	MAllocator::MAllocator()
	{
		// 采用引用计数 确保所有程序退出后 内存得到释放
		MAllocator::object_num += 1;
		//atexit(MAllocator::callCMF);
	}
	MAllocator::~MAllocator()
	{
		MAllocator::object_num -= 1;
		if (MAllocator::object_num == 0)
		{
			MAllocator::callCMF();
		}
	}
	void MAllocator::callCMF()
	{
		for (int i = 0; i < __MUZI_ALLOCATOR_MOD_SIZE__; ++i)
		{
			if(MAllocator::cmf[i] != nullptr)
				MAllocator::cmf[i](MAllocator::cmf_arg[i]);
		}
	}
	void MAllocator::setClearMemoryFunction(clearMemoryFunction fun, void* arg)
	{
		for (int i = 0; i < __MUZI_ALLOCATOR_MOD_SIZE__; ++i)
		{
			if (MAllocator::cmf[i] == nullptr)
			{
				MAllocator::cmf[i] = fun;
				MAllocator::cmf_arg[i] = arg;
				break;
			}
		}
	}

#ifdef __MUZI_ALLOCATOR_MOD_POOL__
	size_t MAllocator::object_num = 0;
	size_t MAllocator::pool_mem_total = 0;
	union MAllocator::MAllocatorRep* MAllocator::pool_mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__] = { nullptr };
	union MAllocator::MAllocatorRep* MAllocator::pool_start_free_pool_ptr = nullptr;
	union MAllocator::MAllocatorRep* MAllocator::pool_end_free_pool_ptr = nullptr;
	MAllocator::clearMemoryFunction MAllocator::cmf[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	void* MAllocator::cmf_arg[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };

	inline size_t MAllocator::pool_RoundUp(size_t bytes)
	{
		return 
			(bytes + __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1)
			& -(__MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1);
	}
	inline size_t MAllocator::pool_freelist_index(size_t bytes)
	{
		return (bytes + __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1) / __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1;
	}
	inline bool MAllocator::pool_is_possible_mem_board(void* p)
	{
		return (*((char*)p) == __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__) ? true : false;
	}

	void* MAllocator::pool_allocate(size_t type_size)
	{
		// 大于上界 即转交给malloc去申请 此时malloc所多出的块相比于数据本身已经很小
		if (type_size > (size_t)__MUZI_ALLOCAOTR_MOD_POOL_MAX_SPECIFICATION__)
		{
			return ::operator new(type_size);
		}

		MAllocatorRep* ret_mem_ptr = nullptr;
		size_t pool_index = MAllocator::pool_freelist_index(type_size); // 由大小（八的倍数）获取其坐标
		size_t pool_index_specification = (pool_index + 1) * __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__; // 获取当前位置对应的内存申请规格

		constexpr size_t alloc_len = __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__;

		// 发现该点之前未分配或者该点之前分配的分配完了 
		if (MAllocator::pool_mem_pool[pool_index] == nullptr)
		{
			// 并且战备池已经空了或者从未申请
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
				list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
				MAllocatorRep* list_next_ptr_temp = list_next_ptr;

				// 构建空闲单向链表来切割内存
				for (int i = 0; i < __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__; ++i)
				{
					list_next_ptr =
						(MAllocatorRep*)((char*)list_next_ptr - pool_index_specification);
					list_next_ptr->next = list_next_ptr_temp;
					list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
					list_next_ptr_temp = list_next_ptr;
				}

				ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
				MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_mem_pool[pool_index]->next;
			}
			else// 战备池有剩余
			{
				size_t pool_size = 
					(MAllocator::pool_end_free_pool_ptr - MAllocator::pool_start_free_pool_ptr) / 8;
				if (type_size <= pool_size)
				{
					size_t pool_block_count = pool_size / type_size;// 可以分割多少个内存块
					size_t residue_mem = pool_size % type_size;// 剩余内存
					MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_start_free_pool_ptr;
					// 分割内存 从头往后分割
					MAllocatorRep* list_next_ptr =
						static_cast<MAllocatorRep*>(MAllocator::pool_start_free_pool_ptr);
					MAllocatorRep* list_next_ptr_temp = list_next_ptr;// 临时指针
					for (int i = 0; i < pool_block_count; ++i)
					{
						list_next_ptr_temp =
							(MAllocatorRep*)((char*)list_next_ptr + pool_index_specification);
						list_next_ptr->next = list_next_ptr_temp;
						list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
						list_next_ptr = list_next_ptr_temp;
					}

					list_next_ptr->next = nullptr;
					list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
				}
				else
				{

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

		if(ret_mem_ptr != nullptr)
			ret_mem_ptr->data[0] = 0;
		return static_cast<void*>(ret_mem_ptr);
	}
	void MAllocator::pool_deallocate(void** ptr, size_t mem_size)
	{
		size_t mem_index = MAllocator::pool_freelist_index(mem_size);
		MAllocatorRep* tmp_ptr = MAllocator::pool_mem_pool[mem_index];
		MAllocatorRep* giveback_ptr = static_cast<MAllocatorRep*>(*ptr);
		// 将归还内容置于顶部   
		giveback_ptr->next = tmp_ptr;
		giveback_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;
		MAllocator::pool_mem_pool[mem_index] = giveback_ptr;
		*ptr = nullptr;// 将指针指向空 使得在delete后不会被越权访问
	}
	void* MAllocator::pool_reallocate()
	{
		return nullptr;
	}

#endif // __MUZI_ALLOCATOR_MOD_POOL__


};
