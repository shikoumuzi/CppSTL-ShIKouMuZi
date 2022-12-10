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

	size_t MAllocator::object_num																				= 0;
	size_t MAllocator::pool_mem_total																			= 0;
	size_t MAllocator::pool_mem_from_sys_total																	= 0;
	union MAllocator::MAllocatorRep* MAllocator::pool_mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__] = { nullptr };
	union MAllocator::MAllocatorRep* MAllocator::pool_start_free_pool_ptr										= nullptr;
	union MAllocator::MAllocatorRep* MAllocator::pool_end_free_pool_ptr											= nullptr;
	MAllocator::clearMemoryFunction MAllocator::cmf[__MUZI_ALLOCATOR_MOD_SIZE__]								= { nullptr };
	void* MAllocator::cmf_arg[__MUZI_ALLOCATOR_MOD_SIZE__]														= { nullptr };

	// 规约内存大小
	inline size_t MAllocator::pool_RoundUp(size_t bytes)
	{
		return (bytes + __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ - 1)
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
	MAllocator::MAllocatorRep* MAllocator::pool_mem_split(MAllocatorRep* start_ptr, size_t mem_specification, size_t mem_block_count)
	{
		// 将申请的内存进行分配（从头开始）
		MAllocatorRep* list_next_ptr		= start_ptr;
		MAllocatorRep* list_next_ptr_temp	= list_next_ptr;// 临时指针
		// 构建空闲单向链表来切割内存
		for (int i = 0; i < mem_block_count; ++i)
		{
			// 将内存按规格大小划分
			list_next_ptr_temp		= (MAllocatorRep*)((char*)list_next_ptr + mem_specification);
			list_next_ptr->next		= list_next_ptr_temp;
			list_next_ptr->data[0]  |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
			list_next_ptr			= list_next_ptr_temp;
		}
		// 尾部添加双终止符
		list_next_ptr->next			= nullptr;
		list_next_ptr->data[0]		|= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// 添加内存标识符
		return list_next_ptr;

	}

	void* MAllocator::pool_allocate(size_t type_size)
	{
		// 大于上界 即转交给malloc去申请 此时malloc所多出的块相比于数据本身已经很小
		if (type_size > (size_t)__MUZI_ALLOCAOTR_MOD_POOL_MAX_SPECIFICATION__)
		{
			return ::operator new(type_size);
		}

		MAllocatorRep* ret_mem_ptr			= nullptr;
		size_t pool_index					= MAllocator::pool_freelist_index(type_size); // 由大小（八的倍数）获取其坐标
		size_t pool_index_specification		= __MUZI_ALLOCAOTR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(pool_index); // 获取当前位置对应的内存申请规格

		constexpr size_t alloc_len			= __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__;
		// 如果该池子的最大申请量已经不足以再去分配一个，那就从数组右边更大处调配过来一个
		if (__MUZI_ALLOCAOTR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MAllocator::pool_mem_total < type_size)
		{
			// 缺少
			size_t free_pool_index			= MAllocator::pool_freelist_index(type_size);
			size_t apply_mem_index			= free_pool_index + 1;
			size_t apply_mem_block_count	= 0;// 一个大内存块符合type_size需要多少块的量
			size_t apply_mem_block_index	= 0;// 大内存块计算
			if (MAllocator::pool_mem_pool[free_pool_index] != nullptr)
			{
				goto POOL_NO_RESIDUAL_VALUE;
			}
			// 检索仍然有剩余的内容
			for (apply_mem_index; apply_mem_index < __MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__; ++apply_mem_index)
			{
				if (MAllocator::pool_mem_pool[apply_mem_index] != nullptr)
				{
					size_t mem_block_total = 0;
					for (apply_mem_block_index; apply_mem_block_count;)
					{

					}
					break;
				}
			}




			if (ret_mem_ptr != nullptr)
				ret_mem_ptr->data[0] = 0;
			return static_cast<void*>(ret_mem_ptr);
		}

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
				MAllocator::pool_start_free_pool_ptr	= MAllocator::pool_mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MAllocator::pool_end_free_pool_ptr		= MAllocator::pool_start_free_pool_ptr + alloc_len * type_size;

				// 记录申请总量
				MAllocator::pool_mem_from_sys_total		+= alloc_len * type_size * 2;

				// 将申请到的内存进行切割
				MAllocator::pool_mem_split
				(MAllocator::pool_mem_pool[pool_index], pool_index_specification, __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__);

				ret_mem_ptr								= MAllocator::pool_mem_pool[pool_index];
				MAllocator::pool_mem_pool[pool_index]	= MAllocator::pool_mem_pool[pool_index]->next;
				MAllocator::pool_mem_total				+= pool_index_specification;
			}
			else// 战备池有剩余
			{
				size_t pool_size = /*计算剩余数 并除以对应bit-字节换算*/
					(MAllocator::pool_end_free_pool_ptr - MAllocator::pool_start_free_pool_ptr) / 8;
				if (type_size <= pool_size)// 有剩余且大于需求量
				{
					size_t pool_block_count					= pool_size / type_size;// 可以分割多少个内存块
					size_t residue_mem						= pool_size % type_size;// 剩余内存
					MAllocator::pool_mem_pool[pool_index]	= MAllocator::pool_start_free_pool_ptr;
					// 分割内存 从头往后分割

					MAllocator::pool_mem_split
					(MAllocator::pool_start_free_pool_ptr, pool_index_specification, pool_block_count);

					ret_mem_ptr								= MAllocator::pool_mem_pool[pool_index];
					MAllocator::pool_mem_pool[pool_index]	= MAllocator::pool_mem_pool[pool_index]->next;
					MAllocator::pool_mem_total				+= pool_index_specification;
				}
				else if(type_size > __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__)// 有剩余但小于需求量 且大于最小值
				{
					size_t free_pool_2_mem_pool_index		= MAllocator::pool_freelist_index(pool_size);
					size_t free_pool_index_specification	= __MUZI_ALLOCAOTR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(free_pool_2_mem_pool_index);
					size_t free_pool_block_count			= pool_size / free_pool_index_specification;
					MAllocatorRep* last_ptr					=  MAllocator::pool_mem_split
					(MAllocator::pool_mem_pool[free_pool_2_mem_pool_index], free_pool_index_specification, free_pool_block_count);

					if (MAllocator::pool_mem_pool[free_pool_2_mem_pool_index] != nullptr)
					{
						last_ptr->next = MAllocator::pool_mem_pool[free_pool_2_mem_pool_index];
					}

					MAllocator::pool_mem_pool[free_pool_2_mem_pool_index]	= MAllocator::pool_start_free_pool_ptr;
					int difference_value									= (MAllocator::pool_end_free_pool_ptr - last_ptr) / 8;

					if (difference_value != 0)// 保留至战备池
					{
						MAllocator::pool_start_free_pool_ptr = MAllocator::pool_end_free_pool_ptr + free_pool_index_specification - free_pool_index_specification * __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__;
					}
					else// 重新规约
					{
						MAllocator::pool_start_free_pool_ptr	= nullptr;
						MAllocator::pool_end_free_pool_ptr		= nullptr;
					}
				}
				else// 有剩余 但已经小于最小值了
				{
					// 将初始指针和尾指针置为nullptr，重新调用函数使其进入其他分支
					MAllocator::pool_start_free_pool_ptr = nullptr;
					MAllocator::pool_end_free_pool_ptr = nullptr;
					return MAllocator::pool_allocate(type_size);
				}
			}
		}
		else// 如果已经分配过且仍然有空余
		{
			POOL_NO_RESIDUAL_VALUE:
			MAllocatorRep* list_next_ptr			= MAllocator::pool_mem_pool[pool_index]->next;
			ret_mem_ptr								= MAllocator::pool_mem_pool[pool_index];
			MAllocator::pool_mem_pool[pool_index]	= MAllocator::pool_mem_pool[pool_index]->next;

		}

		if(ret_mem_ptr != nullptr)
			ret_mem_ptr->data[0] = 0;
		return static_cast<void*>(ret_mem_ptr);
	}
	void MAllocator::pool_deallocate(void** ptr, size_t mem_size)
	{
		size_t mem_index						= MAllocator::pool_freelist_index(mem_size);
		MAllocatorRep* tmp_ptr					= MAllocator::pool_mem_pool[mem_index];
		MAllocatorRep* giveback_ptr				= static_cast<MAllocatorRep*>(*ptr);
		// 将归还内容置于顶部   
		giveback_ptr->next						= tmp_ptr;
		giveback_ptr->data[0]					|= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;
		MAllocator::pool_mem_pool[mem_index]	= giveback_ptr;
		*ptr = nullptr;// 将指针指向空 使得在delete后不会被越权访问
	}
	void* MAllocator::pool_reallocate()
	{
		return nullptr;
	}

#endif // __MUZI_ALLOCATOR_MOD_POOL__


};
