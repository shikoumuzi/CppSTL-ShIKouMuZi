#include"MPoolAllocator.h"
namespace MUZI {
	static std::atomic<int> thread_flag = 0;


	MPoolAllocator::MPoolAllocator()
	{
		this->pool_mem_total = 0;
		this->pool_mem_from_sys_total = 0;
		this->pool_mem_pool[__MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__] = { nullptr };
		this->pool_start_free_pool_ptr = nullptr;
		this->pool_end_free_pool_ptr = nullptr;
		this->sys_memory_block = (MPoolAllocator::MAllocatorRep**)malloc
		(sizeof(MPoolAllocator::MAllocatorRep*) * __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__);
		this->pool_init();
	}
	MPoolAllocator::~MPoolAllocator()
	{
		this->pool_delete();
	}

	void MPoolAllocator::pool_init()
	{
		memset(MPoolAllocator::sys_memory_block, (int)nullptr, __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__);
	}
	void MPoolAllocator::pool_delete()
	{
		for (int i = 0; i < __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__; ++i)
		{
			if (MPoolAllocator::sys_memory_block[i] != nullptr)
			{
				free(MPoolAllocator::sys_memory_block[i]);
				MPoolAllocator::sys_memory_block[i] = nullptr;
			}
		}
	}

	size_t MPoolAllocator::pool_get_mem_array_free_index()
	{
		for (int i = 1; i < __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__; ++i)
		{
			if (MPoolAllocator::sys_memory_block[i] == nullptr)
			{
				return i;
			}
		}
		return 0;
	}
	// 规约内存大小
	inline size_t MPoolAllocator::pool_RoundUp(size_t bytes)
	{
		return (bytes + __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1)
			& -(__MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1);
	}
	inline size_t MPoolAllocator::pool_freelist_index(size_t bytes)
	{
		return (bytes + __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1) / __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1;
	}
	inline bool MPoolAllocator::pool_is_possible_mem_board(void* p)
	{
		return *((char*)p) == __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;
	}
	MPoolAllocator::MAllocatorRep* MPoolAllocator::pool_mem_split(MAllocatorRep* start_ptr, size_t mem_specification, size_t mem_block_count)
	{
		// 将申请的内存进行分配（从头开始）
		MAllocatorRep* list_next_ptr = start_ptr;
		MAllocatorRep* list_next_ptr_temp = list_next_ptr;// 临时指针
		// 构建空闲单向链表来切割内存
		for (int i = 0; i < mem_block_count; ++i)
		{
			// 将内存按规格大小划分
			list_next_ptr_temp = (MAllocatorRep*)((char*)list_next_ptr + mem_specification);
			list_next_ptr->next = list_next_ptr_temp;
			list_next_ptr->data[0] |= __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
			list_next_ptr = list_next_ptr_temp;
		}
		// 尾部添加双终止符
		list_next_ptr->next = nullptr;
		list_next_ptr->data[0] |= __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
		return list_next_ptr;

	}
	inline bool MPoolAllocator::is_error_sys_mem(void** mem_ptr)
	{
		return (&mem_ptr == (void*)MPoolAllocator::sys_memory_block) ? (delete * mem_ptr, *mem_ptr = nullptr, true) : false;
	}

	inline void* MPoolAllocator::pool_apply_mem_from_sys(size_t mem_size)
	{
		// 如果剩余可申请量大于需求量，那么可以直接从系统申请并记录申请总量，否则返回nullptr告诉调用者无法再向系统申请，同时将调用内存量及时记录，以便后续统一释放
		return (__MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MPoolAllocator::pool_mem_from_sys_total > mem_size) ? \
			(MPoolAllocator::pool_mem_from_sys_total += mem_size, MPoolAllocator::sys_memory_block[MPoolAllocator::pool_get_mem_array_free_index()] = (MAllocatorRep*)::operator new(mem_size)) : nullptr;
	}

	void* MPoolAllocator::pool_allocate(size_t type_size)
	{
		// 大于上界 即转交给malloc去申请 此时malloc所多出的块相比于数据本身已经很小
		if (type_size > (size_t)__MUZI_ALLOCATOR_MOD_POOL_MAX_SPECIFICATION__)
		{
			return ::operator new(type_size);
		}

		MAllocatorRep* ret_mem_ptr = nullptr;
		size_t pool_index = MPoolAllocator::pool_freelist_index(type_size); // 由大小（八的倍数）获取其坐标
		size_t pool_index_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(pool_index); // 获取当前位置对应的内存申请规格

		constexpr size_t alloc_len = __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__;
		// 如果该池子的最大申请量已经不足以再去分配一个，那就从数组右边更大处调配过来一个
		//if (__MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MPoolAllocator::pool_mem_total < type_size)
		//{
		//	// 缺少
		//	size_t free_pool_index					= MPoolAllocator::pool_freelist_index(type_size);
		//	size_t apply_mem_index					= free_pool_index + 1;
		//	size_t apply_mem_block_count			= 0;// 一个大内存块符合type_size需要多少块的量
		//	size_t apply_mem_block_index			= 0;// 大内存块计算
		//	size_t apply_mem_block_specification	= 0;
		//	MAllocatorRep* free_mem_ptr				= nullptr;
		//	if (MPoolAllocator::pool_mem_pool[free_pool_index] != nullptr)
		//	{
		//		goto POOL_NO_RESIDUAL_VALUE;
		//	}
		//	// 检索仍然有剩余的内容
		//	for (apply_mem_index; apply_mem_index < __MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__; ++apply_mem_index)
		//	{
		//		if (MPoolAllocator::pool_mem_pool[apply_mem_index] != nullptr)
		//		{
		//			free_mem_ptr = MPoolAllocator::pool_mem_pool[apply_mem_index];
		//			size_t mem_block_total = 0;
		//			apply_mem_block_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(apply_mem_block_index);
		//			for (apply_mem_block_index; free_mem_ptr != nullptr; ++apply_mem_block_index, apply_mem_block_count += apply_mem_block_specification, free_mem_ptr = free_mem_ptr->next)
		//			{
		//				if (apply_mem_block_count >= type_size)
		//				{

		//				}
		//			}
		//			break;
		//		}
		//	}
		//	if (ret_mem_ptr != nullptr)
		//		ret_mem_ptr->data[0] = 0;
		//	return static_cast<void*>(ret_mem_ptr);
		//}
		// 功能不确定，暂且不实现

		// 发现该点之前未分配或者该点之前分配的分配完了 
		if (MPoolAllocator::pool_mem_pool[pool_index] == nullptr)
		{
			// 并且战备池已经空了或者从未申请
			if (MPoolAllocator::pool_start_free_pool_ptr == MPoolAllocator::pool_end_free_pool_ptr)
			{
				// 一半用以正常申请内容 一半用以战备池
				MPoolAllocator::pool_mem_pool[pool_index] = static_cast<MAllocatorRep*>
					(MPoolAllocator::pool_apply_mem_from_sys(alloc_len * type_size * 2));

				// 获取战备池
				MPoolAllocator::pool_start_free_pool_ptr = MPoolAllocator::pool_mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MPoolAllocator::pool_end_free_pool_ptr = (MAllocatorRep*)((char*)MPoolAllocator::pool_start_free_pool_ptr + alloc_len * type_size);

				// 将申请到的内存进行切割
				MPoolAllocator::pool_mem_split
				(MPoolAllocator::pool_mem_pool[pool_index], pool_index_specification, __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__);

				ret_mem_ptr = MPoolAllocator::pool_mem_pool[pool_index];
				MPoolAllocator::pool_mem_pool[pool_index] = MPoolAllocator::pool_mem_pool[pool_index]->next;
				MPoolAllocator::pool_mem_total += pool_index_specification;
			}
			else// 战备池有剩余
			{
				size_t pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__(MPoolAllocator::pool_start_free_pool_ptr, MPoolAllocator::pool_end_free_pool_ptr);
				if (type_size <= pool_size)// 有剩余且大于需求量
				{
					size_t pool_block_count = pool_size / type_size;// 可以分割多少个内存块
					size_t residue_mem = pool_size % type_size;// 剩余内存
					MPoolAllocator::pool_mem_pool[pool_index] = MPoolAllocator::pool_start_free_pool_ptr;
					// 分割内存 从头往后分割
					// 计算指针
					MAllocatorRep* last_ptr = MPoolAllocator::pool_mem_split
					(MPoolAllocator::pool_start_free_pool_ptr, pool_index_specification, pool_block_count);
					//分配内存出去
					ret_mem_ptr = MPoolAllocator::pool_mem_pool[pool_index];
					MPoolAllocator::pool_mem_pool[pool_index] = MPoolAllocator::pool_mem_pool[pool_index]->next;
					MPoolAllocator::pool_mem_total += pool_index_specification;
					MPoolAllocator::pool_start_free_pool_ptr = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_STATR_PTR_BY_LAST_ELEMENT__\
						(MPoolAllocator::pool_end_free_pool_ptr, last_ptr, pool_index_specification);
					pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__\
						(MPoolAllocator::pool_start_free_pool_ptr, MPoolAllocator::pool_end_free_pool_ptr);
				}
				else if (type_size > __MUZI_ALLOCATOR_MOD_POOL_ALIGN__)// 有剩余但小于需求量 且大于最小值
				{
					//这里先将剩余量不断分割嫁接到其他内存位置，直到小于最小规格为止，舍弃内存碎片，然后在为需求内存重新申请分配，再重组战备池
					while (pool_size != 0)
					{
						size_t free_pool_2_mem_pool_index = MPoolAllocator::pool_freelist_index(pool_size);
						size_t free_pool_index_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(free_pool_2_mem_pool_index);
						size_t free_pool_block_count = pool_size / free_pool_index_specification;
						MAllocatorRep* last_ptr = MPoolAllocator::pool_mem_split
						(MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index], free_pool_index_specification, free_pool_block_count);

						if (MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index] != nullptr)
						{
							// 把剩余值接到匹配位置去
							last_ptr->next = MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index];
						}

						MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index] = MPoolAllocator::pool_start_free_pool_ptr;
						int difference_value = ((char*)MPoolAllocator::pool_end_free_pool_ptr - (char*)last_ptr) / 8;

						if (difference_value != 0)// 将剩余碎片保留至战备池
						{
							MPoolAllocator::pool_start_free_pool_ptr = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_STATR_PTR_BY_LAST_ELEMENT__\
								(MPoolAllocator::pool_end_free_pool_ptr, last_ptr, free_pool_index_specification);
							pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__\
								(MPoolAllocator::pool_start_free_pool_ptr, MPoolAllocator::pool_end_free_pool_ptr);
							if (pool_size < __MUZI_ALLOCATOR_MOD_POOL_MIN_SPECIFICATION__)
							{
								goto POOL_SIZE_TO_SET_0;
							}
						}
						else// 重新规约
						{
						POOL_SIZE_TO_SET_0:
							MPoolAllocator::pool_start_free_pool_ptr = nullptr;
							MPoolAllocator::pool_end_free_pool_ptr = nullptr;
							pool_size = 0;
						}
					}
					// 这里重新为其申请内存
					MPoolAllocator::pool_mem_pool[pool_index] = (MAllocatorRep*)MPoolAllocator::pool_apply_mem_from_sys\
						(type_size + __MUZI_ALLOCATOR_MOD_POOL_ROUNDUP__(MPoolAllocator::pool_mem_from_sys_total));
					if (MPoolAllocator::pool_mem_pool[pool_index] != nullptr && !MPoolAllocator::is_error_sys_mem((void**)&MPoolAllocator::pool_mem_pool[pool_index]))//申请成功
					{
						size_t mem_index = MPoolAllocator::pool_freelist_index(type_size);
						size_t mem_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(mem_index);
						MAllocatorRep* end_ptr = MPoolAllocator::pool_mem_split(MPoolAllocator::pool_mem_pool[pool_index], mem_specification, type_size / mem_specification);

						ret_mem_ptr = MPoolAllocator::pool_mem_pool[pool_index];
						pool_mem_pool[pool_index] = pool_mem_pool[pool_index]->next;
						MPoolAllocator::pool_start_free_pool_ptr = (MAllocatorRep*)((char*)end_ptr + mem_specification);
						MPoolAllocator::pool_end_free_pool_ptr = (MAllocatorRep*)((char*)MPoolAllocator::pool_start_free_pool_ptr + __MUZI_ALLOCATOR_MOD_POOL_ROUNDUP__(MPoolAllocator::pool_mem_from_sys_total));
						goto POOL_ALLOCATE_END;
					}
					ret_mem_ptr = nullptr;
				}
				else// 有剩余 但已经小于最小值了
				{
					// 将初始指针和尾指针置为nullptr，重新调用函数使其进入其他分支
					MPoolAllocator::pool_start_free_pool_ptr = nullptr;
					MPoolAllocator::pool_end_free_pool_ptr = nullptr;
					return MPoolAllocator::pool_allocate(type_size);
				}
			}
		}
		else// 如果已经分配过且仍然有空余
		{
			ret_mem_ptr = MPoolAllocator::pool_mem_pool[pool_index];
			MPoolAllocator::pool_mem_pool[pool_index] = MPoolAllocator::pool_mem_pool[pool_index]->next;
			MPoolAllocator::pool_mem_total += __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(pool_index);
		}
	POOL_ALLOCATE_END:
		if (ret_mem_ptr != nullptr)
			ret_mem_ptr->data[0] = 0;
		return static_cast<void*>(ret_mem_ptr);
	}
	void MPoolAllocator::pool_deallocate(void* ptr, size_t mem_size)
	{
		size_t mem_index = MPoolAllocator::pool_freelist_index(mem_size);
		MAllocatorRep* tmp_ptr = MPoolAllocator::pool_mem_pool[mem_index];
		MAllocatorRep* giveback_ptr = static_cast<MAllocatorRep*>(ptr);
		// 将归还内容置于顶部   
		giveback_ptr->next = tmp_ptr;
		giveback_ptr->data[0] |= __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;
		MPoolAllocator::pool_mem_pool[mem_index] = giveback_ptr;
		MPoolAllocator::pool_mem_total -= mem_size;
		ptr = nullptr;// 将指针指向空 使得在delete后不会被越权访问
	}
	void* MPoolAllocator::pool_reallocate()
	{
		return nullptr;
	}
}