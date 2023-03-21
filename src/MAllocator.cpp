#include"MAllocator.h"

namespace MUZI {
#ifdef __MUZI_ALLOCATOR_MOD_POOL__
#elif define __MUZI_ALLOCATOR_MOD_LOKI__
#elif define __MUZI_ALLOCATOR_MOD_BITMAP__
#endif // __MUZI_ALLOCATOR_MOD_POOL__


	MAllocator::MemoryCtrlFunction MAllocator::mcf[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	void* MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	MAllocator::clearMemoryFunction MAllocator::cmf[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	void* MAllocator::cmf_arg[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	size_t MAllocator::object_num = 0;
	MAllocator::MAllocator()
	{
		if (object_num == 0)
		{
#ifdef __MUZI_ALLOCATOR_MOD_POOL__
			MAllocator::mcf[__MUZI_ALLOCATOR_MOD_POOL__] = MAllocator::pool_init;
			MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_POOL__] = nullptr;
#endif
#ifdef __MUZI_ALLOCATOR_MOD_BITMAP__
			MAllocator::mcf[__MUZI_ALLOCATOR_MOD_BITMAP__] = MAllocator::bitmap_init;
			MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_BITMAP__] = nullptr;
#endif
#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
			MAllocator::mcf[__MUZI_ALLOCATOR_MOD_FIXED__] = MAllocator::fixed_init;
			MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_FIXED__] = nullptr;
#endif
#ifdef __MUZI_ALLOCATOR_MOD_ARRAY__

#endif // __MUZI_ALLOCATOR_MOD_ARRAY__


		}
		// 采用引用计数 确保所有程序退出后 内存得到释放
		MAllocator::object_num += 1;
		atexit(MAllocator::atexitDestruct);
	}
	MAllocator::~MAllocator()
	{
		MAllocator::object_num -= 1;
		if (MAllocator::object_num == 0)
		{
			for (int i = 0; i < __MUZI_ALLOCATOR_MOD_SIZE__; ++i)
			{
				if (MAllocator::cmf[i] != nullptr)
					MAllocator::cmf[i](MAllocator::cmf_arg[i]);
			}
		}
	}
	void MAllocator::atexitDestruct()
	{
		for (int i = 0; i < __MUZI_ALLOCATOR_MOD_SIZE__; ++i)
		{
			if (MAllocator::cmf[i] != nullptr)
				MAllocator::cmf[i](MAllocator::cmf_arg[i]);
		}
	}


#ifdef __MUZI_ALLOCATOR_MOD_POOL__
	static std::atomic<int> thread_flag = 0;
	size_t MPoolAllocator::pool_mem_total = 0;
	size_t MPoolAllocator::pool_mem_from_sys_total = 0;
	union MPoolAllocator::MAllocatorRep* MPoolAllocator::pool_mem_pool[__MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__] = { nullptr };
	union MPoolAllocator::MAllocatorRep* MPoolAllocator::pool_start_free_pool_ptr = nullptr;
	union MPoolAllocator::MAllocatorRep* MPoolAllocator::pool_end_free_pool_ptr = nullptr;
	MPoolAllocator::MAllocatorRep** MPoolAllocator::sys_memory_block = (MPoolAllocator::MAllocatorRep**)malloc
	(sizeof(MPoolAllocator::MAllocatorRep*) * __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__);

	void MPoolAllocator::pool_init(void*)
	{
		memset(MPoolAllocator::sys_memory_block, (int)nullptr, __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__);
	}
	void MPoolAllocator::pool_delete(void*)
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
		return (&mem_ptr == (void*)MPoolAllocator::sys_memory_block)?(delete *mem_ptr, *mem_ptr = nullptr, true):false;
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

#endif // __MUZI_ALLOCATOR_MOD_POOL__

#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
	// Chunk管理模块（最底层）
	MLOKIAllocator::MFixedAllocator::MChunk::MChunk()
	{
		this->p_data = nullptr;
		this->first_available_block = 0;
		this->blocks_available = 0;
	}
	MLOKIAllocator::MFixedAllocator::MChunk::~MChunk()
	{
		delete[] this->p_data;
	}
	void MLOKIAllocator::MFixedAllocator::MChunk::Init(size_t block_size, unsigned char block_num)
	{
		this->p_data = new unsigned char[block_num * block_size];
		Reset(block_size, block_num);
	}
	void MLOKIAllocator::MFixedAllocator::MChunk::Reset(size_t block_size, unsigned char block_num)
	{
		this->first_available_block = 0;
		this->blocks_available = block_num;

		// 创造嵌入式指针
		unsigned char i = 0;
		unsigned char* p = this->p_data;
		for (; i != block_num; p += block_size)
			*p = ++i;
		// 在每个分配内存块里头的前一个字节中记录内存块的索引号
	}
	void MLOKIAllocator::MFixedAllocator::MChunk::Release()
	{
		delete[] this->p_data;// 释放自己
		p_data = nullptr;// 解除索引 
	}
	void* MLOKIAllocator::MFixedAllocator::MChunk::Allocate(size_t block_size)
	{
		if (!this->blocks_available)
			return nullptr;
		// 跳转到当前可分配出去的内存块
		unsigned char* p_result = this->p_data + (this->first_available_block * block_size);
		this->first_available_block = *p_result;
		--blocks_available;// 更改可用数目
		
		return p_result;
	}
	void MLOKIAllocator::MFixedAllocator::MChunk::Deallocate(void* p, size_t block_num)
	{
		// 该函数由上层函数确定是落在该chunk后，再调用该函数进行回收资源
		unsigned char* to_release = static_cast<unsigned char*>(p);

		*to_release = this->first_available_block;
		this->first_available_block = static_cast<unsigned char>(to_release - this->p_data) / block_num;
		++this->blocks_available;
	}

	// FixedAllocator(中间件)
	MLOKIAllocator::MFixedAllocator::MFixedAllocator(size_t block_size, unsigned char block_num)
	{
		this->block_size = block_size;
		this->block_num = block_num;
		this->alloc_chunk = nullptr;
		this->dealloc_chunk = nullptr;
		this->chunks.reserve(block_num); 
	}
	MLOKIAllocator::MFixedAllocator::~MFixedAllocator()
	{
		this->alloc_chunk = nullptr;
		this->dealloc_chunk = nullptr;
	}
	void* MLOKIAllocator::MFixedAllocator::Allocate()
	{
		if (this->alloc_chunk != nullptr || this->alloc_chunk->blocks_available != 0)
		{
			// 目前没有标定chunk或者该chunk已经没有空闲区块了
			for (auto chunk : this->chunks)
			{
				if (chunk.blocks_available > 0)
				{
					// 有可用区块
					this->alloc_chunk = &chunk;
					goto __MAllocator_MFixedAllocator_Allocate_Ret__;
				}
			}
			this->chunks.emplace_back(MChunk());
			MChunk& newChunk = this->chunks.back();// 指向末端chunk
			newChunk.Init(this->block_size, this->block_num);// 设置索引
			this->alloc_chunk = &newChunk;// 标定，稍后调用该区块allocate函数获取内存块
			dealloc_chunk = &this->chunks.front();// 因为vector可能内部会对数据进行搬动，每次都需要重新设置，所以需要标定头）
		}
__MAllocator_MFixedAllocator_Allocate_Ret__:
		return alloc_chunk->Allocate(block_size);
		// 在此找到chunk后下次优先在此找起
	}
	void* MLOKIAllocator::MFixedAllocator::Deallocate(void* p)
	{
		this->dealloc_chunk = this->VicinityFind(p);
		if (this->dealloc_chunk == nullptr)
		{
			return nullptr;
		}
		this->DoDeallocate(p);
		return static_cast<void*>(this->dealloc_chunk);
	}
	MLOKIAllocator::MFixedAllocator::MChunk* MLOKIAllocator::MFixedAllocator::VicinityFind(void *p)// 临近查找法，由LOKI作者撰写
	{
		const size_t chunk_length =  this->block_size * this->block_num;
		unsigned char* p_tmp = static_cast<unsigned char*>(p);

		// 采用同VC6中malloc查找的方式 通过首地址 + 整个区块大小 得出对应指针是否存在于某个Chunk当中
		MChunk* lo = this->dealloc_chunk;// 标识上一次归还
		MChunk* hi = this->dealloc_chunk + 1; // 标识上一次归还的chunk标识后一个标识
		MChunk* lo_bound = &this->chunks.front();// lo查找的终点
		MChunk* hi_bound = &this->chunks.back() + 1;// hi查找的终点

		// 主要思想为从上次回收的地方向两边寻找 会更大概率碰到空的
		while (1)
		{
			if (lo)
			{
				if (p_tmp >= lo->p_data && p_tmp < lo->p_data + chunk_length)
					return lo;
				
				if (lo == lo_bound) lo = nullptr;
				else
					--lo;

			}
			if (hi)
			{
				if (p_tmp >= hi->p_data && p_tmp < hi->p_data + chunk_length)
					return hi;
				
				if (hi == hi_bound) hi = nullptr;
				else
					++hi;
			}
			if (lo == hi)// 只有当两个都为nullptr时才会相等 
			{
				return nullptr;
			}
		}
	}
	void MLOKIAllocator::MFixedAllocator::DoDeallocate(void* p)
	{
		this->dealloc_chunk->Deallocate(p, this->block_size);
		if (this->dealloc_chunk->blocks_available == this->block_num)
		{
			// 因为是要找全归还的chunk 所以back()一定是一个空闲块
			MChunk& last_chunk = this->chunks.back();
			// 确认在该Chunk中内存块已经全部回收
			// 确保有两个全回收的内存块后才会归还一个给操作系统
			if (&last_chunk == dealloc_chunk)
			{
				// 如果当前dealloc的chunk正是已经全回收的第二个
				if (this->chunks.size() > 1 &&
					(this->dealloc_chunk - 1)->blocks_available == this->block_num)
				{
					// 如果
					last_chunk.Release();
					this->chunks.pop_back();
					this->alloc_chunk = this->dealloc_chunk = &this->chunks.front();
				}
				return;
			}
			if (last_chunk.blocks_available == this->block_num)
			{
				// 有两个全回收的 chunk就直接抛弃最后一个
				last_chunk.Release();
				chunks.pop_back();
				this->alloc_chunk = this->dealloc_chunk;
				// 此时回收的并不是最后一个入队的
			}
			else
			{
				// 代表并没有两个全回收的模块
				std::swap(*dealloc_chunk, last_chunk);
				this->alloc_chunk = &this->chunks.back();
			}
		}
	}

	// LOKI最上层构建

	void MLOKIAllocator::fixed_init(void*)
	{
		this->max_object_size = __MUZI_ALLOCATOR_MOD_LOKI_MAX_OBJECT_SIZE__;
		this->chunk_size = __MUZI_ALLOCATOR_MOD_LOKI_CHUNK_SIZE__;
	}
	void MLOKIAllocator::fixed_delete()
	{}
	size_t MLOKIAllocator::find_matched_fixedallocate(size_t block_size)
	{
		size_t block_volume = block_size;// 申请内存大小
		size_t applied_pos = 0;// 申请内存块大小所匹配位置
		while (applied_pos >= this->max_object_size)
		{
			block_volume >>= 1;
			applied_pos += 1;
		}
		if (this->fixedallocate_pools[applied_pos].block_size < block_volume)
		{
			applied_pos += 1;
		}
		return applied_pos;
	}
	void* MLOKIAllocator::fixed_allocate(size_t block_size)
	{
		if (block_size > this->max_object_size)
		{
			return ::operator new(block_size);
		}
		size_t applized_pos = this->find_matched_fixedallocate(block_size);
		return this->fixedallocate_pools[block_size].Allocate();
	}
	void MLOKIAllocator::fixed_deallocate(void* p)
	{
		for (auto pool : this->fixedallocate_pools)
		{
			pool.Deallocate(p);
		}
	}
#endif // __MUZI_ALLOCATOR_MOD_LOKI__


#ifdef __MUZI_ALLOCATOR_MOD_BITMAP__
	//// BitMapVector
	//MAllocator::BitMapVector::BitMapVector():p_data(nullptr),capacity(0)
	//{}
	//MAllocator::BitMapVector::BitMapVector(size_t capacity):p_data(nullptr)//输入的是有多少块
	//{
	//	// 在前件保证capacity为__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__的倍数
	//	this->setCapacity(capacity);
	//}
	//MAllocator::BitMapVector::BitMapVector(BitMapVector&& object) noexcept
	//{
	//	this->p_data = object.p_data;
	//	object.p_data = nullptr;
	//	this->capacity = object.capacity;
	//	object.capacity = 0;
	//}
	//MAllocator::BitMapVector::~BitMapVector()
	//{
	//	if (nullptr != this->p_data)
	//	{
	//		delete[] this->p_data->p_bitmap;
	//		this->p_data->p_bitmap = nullptr;
	//		this->p_data->p_start = this->p_data->p_end 
	//			= this->p_data->p_end_storage = this->p_data->p_bitmap = nullptr;
	//		delete this->p_data;
	//		this->p_data = nullptr;
	//	}
	//}
	//void MAllocator::BitMapVector::setCapacity(size_t capacity)
	//{
	//	this->capacity = 0;
	//	if (__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__ * capacity < __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_SIZE__)
	//		this->capacity = capacity;
	//}
	//bool MAllocator::BitMapVector::isValid()
	//{
	//	return  0 == this->capacity || nullptr == this->p_data;
	//}
	//bool MAllocator::BitMapVector::isNull()
	//{
	//	if (nullptr == this->p_data)
	//	{
	//		this->bitmap_size = this->capacity / __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
	//		this->p_data->p_bitmap = new __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__[this->capacity + this->bitmap_size];
	//		this->p_data = new BitMapVectorData;
	//		this->p_data->p_start = this->p_data->p_bitmap + this->bitmap_size;
	//		this->p_data->p_end = this->p_data->p_end;
	//		this->p_data->p_end_storage = this->p_data->p_start + this->capacity + 1;
	//		memset(this->p_data->p_start, 0, this->capacity);
	//		memset(this->p_data->p_bitmap, 0, this->bitmap_size);
	//		return true;
	//	}
	//	return false;
	//}
	//bool MAllocator::BitMapVector::isAllDealloced()
	//{
	//	if(this->p_data == nullptr)
	//		return false;
	//	uint64_t tmp_full_bit = static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__>(-1);
	//	for (size_t i = 0; i < this->bitmap_size; ++i)
	//	{
	//		if (0 != (this->p_data->p_bitmap[i] | tmp_full_bit))
	//		{
	//			return false;
	//		}
	//	}
	//	return true;
	//}
	//bool MAllocator::BitMapVector::isNoFull()
	//{
	//	uint64_t nops = static_cast<uint64_t>(-1);
	//	for (size_t i = 0; i < this->bitmap_size; ++i)
	//	{
	//		if ((this->p_data->p_bitmap[i] & nops) != nops)
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//}
	//bool MAllocator::BitMapVector::isSubPointer(void* p)
	//{
	//	__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__* p_tmp = static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__*>(p);
	//	return p_tmp < this->p_data->p_end&& p_tmp >= this->p_data->p_start;
	//}

	//__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__* MAllocator::BitMapVector::operator[](size_t pos)
	//{
	//	if (pos < (this->p_data->p_end - this->p_data->p_start) / __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__ && pos > 0)
	//	{
	//		return &this->p_data->p_start[pos];
	//	}
	//	return nullptr;
	//}
	//int MAllocator::BitMapVector::push_back()
	//{
	//	this->isNull();
	//	uint64_t tmp_bit = (1 << (__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__));
	//	size_t no_full_bitmap = 0;
	//	if ((no_full_bitmap = this->find_no_full_bitmap()) == -1)
	//	{
	//		return -1;
	//	}
	//	size_t pos = 0;
	//	for (; pos < __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__; ++pos)
	//	{
	//		if ((tmp_bit & this->p_data->p_bitmap[no_full_bitmap]) == 0)
	//		{
	//			if (this->p_data->p_bitmap[no_full_bitmap] == 0)
	//			{
	//				// 每开辟一个新的模块使得end+1
	//				// 每回收一次end前一个模块就向前检查，调整end至新的已分配模块
	//				this->p_data->p_end = this->p_data->p_start + pos + 1;
	//			}
	//			this->p_data->p_bitmap[no_full_bitmap] |= tmp_bit;
	//			
	//		}
	//		tmp_bit >>= 1;
	//	}
	//	return pos;
	//}
	//int MAllocator::BitMapVector::find_no_full_bitmap(size_t reverse)
	//{
	//	size_t i = (this->bitmap_size - 1) & reverse;
	//	auto judged_fun = [&i, this]()->bool {
	//		return this->p_data->p_bitmap[i] != static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__>(-1); };

	//	if (reverse)
	//	{
	//		for (; i >= 0; i -= 1)
	//		{
	//			if (judged_fun()) return i;
	//		}
	//	}
	//	else
	//	{
	//		for (; i < this->bitmap_size; i += 1)
	//		{
	//			if(judged_fun()) return i;
	//		}
	//	}

	//	return -1;
	//}
	//void MAllocator::BitMapVector::pop_back()
	//{
	//	if(this->isNull()) return;
	//	this->earse(this->p_data->p_end -= __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__);
	//	uint64_t tmp_bit = (1 << (__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__));
	//	size_t no_full_bitmap = 0;
	//	if ((no_full_bitmap = this->find_no_full_bitmap()) == -1)return;
	//	size_t pos = __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__;
	//	for (; pos  > 0; --pos)
	//	{
	//		if ((tmp_bit & this->p_data->p_bitmap[no_full_bitmap]) == 0)
	//		{
	//			if (this->p_data->p_bitmap[no_full_bitmap] == 0)
	//			{
	//				// 每开辟一个新的模块使得end+1
	//				// 每回收一次end前一个模块就向前检查，调整end至新的已分配模块
	//				this->p_data->p_end = this->p_data->p_start + pos + 1;
	//			}
	//		}
	//		tmp_bit >>= 1;
	//	}
	//}
	//size_t MAllocator::BitMapVector::earse(__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__ *p)
	//{
	//	size_t distance  = (p - this->p_data->p_start) / __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
	//	if (distance > this->capacity)
	//	{
	//		return -1;
	//	}
	//	// 这里记录除法的余数和结果
	//	size_t distance_remainder = distance % __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
	//	size_t distance_result = distance % __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
	//	// 将对应位置为0
	//	uint64_t tmp_bitmap = ~(1 << distance_remainder);
	//	this->p_data->p_bitmap[distance_result] &= tmp_bitmap;
	//}
	//void  MAllocator::BitMapVector::swap(BitMapVector&& object)
	//{
	//	BitMapVectorData* p_tmp = this->p_data;
	//	this->p_data = object.p_data;
	//	object.p_data = p_tmp;
	//}
	//// BitMapVectors
	//MAllocator::BitMapVectors::BitMapVectors():p_data(this->p_data = new BitMapVectorsData)
	//{
	//	this->p_data->p_start = new BitMapVector[__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
	//	this->p_data->p_end = this->p_data->p_start;
	//	this->p_data->p_end_stoage = this->p_data->p_start + __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ + 1;
	//	this->p_data->p_free_list = new BitMapVectorsData * [__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
	//	this->p_data->p_mem_list = new BitMapVectorsData*[__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
	//	memset(this->p_data->p_free_list, reinterpret_cast<int>(nullptr), __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__);
	//	memset(this->p_data->p_mem_list, reinterpret_cast<int>(nullptr), __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__);
	//}
	//MAllocator::BitMapVectors::BitMapVectors(BitMapVectors&& object) noexcept
	//{
	//	BitMapVectorsData* p_tmp = this->p_data;
	//	this->p_data = object.p_data;
	//	object.p_data = p_tmp;
	//}
	//MAllocator::BitMapVectors::~BitMapVectors()
	//{
	//	if (this->p_data != nullptr)
	//	{
	//		delete[] this->p_data->p_start;
	//		delete[] this->p_data->p_mem_list;
	//		delete[] this->p_data->p_free_list;
	//		memset(this->p_data, reinterpret_cast<int>(nullptr), 5);
	//		delete this->p_data;
	//	}
	//}
	//bool MAllocator::BitMapVectors::isNull()
	//{
	//}
	//int MAllocator::BitMapVectors::push_back(size_t array_size)
	//{
	//	size_t pos = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
	//	if (pos > __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ || pos < 0)
	//	{
	//		// 越界情况
	//	}
	//	this->p_data->p_start[pos].setCapacity(array_size);
	//}
	//void MAllocator::BitMapVectors::pop_back()
	//{
	//	
	//}
	//bool MAllocator::BitMapVectors::compare_by_array_size()
	//{

	//}
	//void MAllocator::BitMapVectors::swap()
	//{

	//}
	//void*  MAllocator::BitMapVectors::allocate()
	//{
	//	size_t i = 0;
	//	for (; i < __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__; ++i)
	//	{
	//		if (this->p_data->p_start[i].isNoFull())// 寻找不是全分配的 不是就直接在这分配一块出去
	//		{
	//			return static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__*>(this->p_data->p_start[i][this->p_data->p_start[i].push_back()]);
	//		}
	//		// 如果都是全分配的那就新创建一个
	//		// this->push_back();
	//	}
	//	return nullptr;
	//}
	//void  MAllocator::BitMapVectors::deallocate(void* p)
	//{
	//	size_t bitmap_vector_count = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
	//	for (size_t i = 0; i < bitmap_vector_count; ++i)
	//	{
	//		if (this->p_data->p_start[i].isSubPointer(p))
	//		{
	//			this->p_data->p_start[i].earse(static_cast<uint64_t*>(p));
	//		}
	//	}
	//}
	//void* MAllocator::bitmap_allocate()
	//{

	//}
	//void MAllocator::bitmap_deallocate(void* p)
	//{

	//}
#endif // __MUZI_ALLOCATOR_MOD_BITMAP__



}
