#include"Allocator.h"

namespace MUZI {

	MAllocator::MemoryCtrlFunction MAllocator::mcf[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	void* MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	MAllocator::clearMemoryFunction MAllocator::cmf[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };
	void* MAllocator::cmf_arg[__MUZI_ALLOCATOR_MOD_SIZE__] = { nullptr };

	MAllocator::MAllocator()
	{
		if (object_num == 0)
		{
#ifdef __MUZI_ALLOCATOR_MOD_POOL__
			MAllocator::mcf[__MUZI_ALLOCATOR_MOD_POOL__] = MAllocator::pool_init;
			MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_POOL__] = nullptr;
#elif define __MUZI_ALLOCATOR_MOD_BITMAP__
			MAllocator::mcf[__MUZI_ALLOCATOR_MOD_BITMAP__] = MAllocator::bitmap_init;
			MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_BITMAP__] = nullptr;
#elif define __MUZI_ALLOCATOR_MOD_FIXED__
			MAllocator::mcf[__MUZI_ALLOCATOR_MOD_FIXED__] = MAllocator::fixed_init;
			MAllocator::mcf_arg[__MUZI_ALLOCATOR_MOD_FIXED__] = nullptr;
#endif
		}
		// �������ü��� ȷ�����г����˳��� �ڴ�õ��ͷ�
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

	size_t MAllocator::object_num = 0;
	size_t MAllocator::pool_mem_total = 0;
	size_t MAllocator::pool_mem_from_sys_total = 0;
	union MAllocator::MAllocatorRep* MAllocator::pool_mem_pool[__MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__] = { nullptr };
	union MAllocator::MAllocatorRep* MAllocator::pool_start_free_pool_ptr = nullptr;
	union MAllocator::MAllocatorRep* MAllocator::pool_end_free_pool_ptr = nullptr;
	MAllocator::MAllocatorRep** MAllocator::sys_memory_block = (MAllocator::MAllocatorRep**)malloc
	(sizeof(MAllocator::MAllocatorRep*) * __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__);

	void MAllocator::pool_init(void*)
	{
		memset(MAllocator::sys_memory_block, (int)nullptr, __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__);
	}
	void MAllocator::pool_delete(void*)
	{
		for (int i = 0; i < __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__; ++i)
		{
			if (MAllocator::sys_memory_block[i] != nullptr)
			{
				free(MAllocator::sys_memory_block[i]);
				MAllocator::sys_memory_block[i] = nullptr;
			}
		}
	}

	size_t MAllocator::pool_get_mem_array_free_index()
	{
		for (int i = 1; i < __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__; ++i)
		{
			if (MAllocator::sys_memory_block[i] == nullptr)
			{
				return i;
			}
		}
		return 0;
	}
	// ��Լ�ڴ��С
	inline size_t MAllocator::pool_RoundUp(size_t bytes)
	{
		return (bytes + __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1)
			& -(__MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1);
	}
	inline size_t MAllocator::pool_freelist_index(size_t bytes)
	{
		return (bytes + __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1) / __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ - 1;
	}
	inline bool MAllocator::pool_is_possible_mem_board(void* p)
	{
		return *((char*)p) == __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;
	}
	MAllocator::MAllocatorRep* MAllocator::pool_mem_split(MAllocatorRep* start_ptr, size_t mem_specification, size_t mem_block_count)
	{
		// ��������ڴ���з��䣨��ͷ��ʼ��
		MAllocatorRep* list_next_ptr = start_ptr;
		MAllocatorRep* list_next_ptr_temp = list_next_ptr;// ��ʱָ��
		// �������е����������и��ڴ�
		for (int i = 0; i < mem_block_count; ++i)
		{
			// ���ڴ水����С����
			list_next_ptr_temp = (MAllocatorRep*)((char*)list_next_ptr + mem_specification);
			list_next_ptr->next = list_next_ptr_temp;
			list_next_ptr->data[0] |= __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
			list_next_ptr = list_next_ptr_temp;
		}
		// β�����˫��ֹ��
		list_next_ptr->next = nullptr;
		list_next_ptr->data[0] |= __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
		return list_next_ptr;

	}
	inline bool MAllocator::is_error_sys_mem(void** mem_ptr)
	{
		return (&mem_ptr == (void*)MAllocator::sys_memory_block)?(delete *mem_ptr, *mem_ptr = nullptr, true):false;
	}

	inline void* MAllocator::pool_apply_mem_from_sys(size_t mem_size)
	{
		// ���ʣ�����������������������ô����ֱ�Ӵ�ϵͳ���벢��¼�������������򷵻�nullptr���ߵ������޷�����ϵͳ���룬ͬʱ�������ڴ�����ʱ��¼���Ա����ͳһ�ͷ�
		return (__MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MAllocator::pool_mem_from_sys_total > mem_size) ? \
			(MAllocator::pool_mem_from_sys_total += mem_size, MAllocator::sys_memory_block[MAllocator::pool_get_mem_array_free_index()] = (MAllocatorRep*)::operator new(mem_size)) : nullptr;
	}

	void* MAllocator::pool_allocate(size_t type_size)
	{
		// �����Ͻ� ��ת����mallocȥ���� ��ʱmalloc������Ŀ���������ݱ����Ѿ���С
		if (type_size > (size_t)__MUZI_ALLOCATOR_MOD_POOL_MAX_SPECIFICATION__)
		{
			return ::operator new(type_size);
		}

		MAllocatorRep* ret_mem_ptr = nullptr;
		size_t pool_index = MAllocator::pool_freelist_index(type_size); // �ɴ�С���˵ı�������ȡ������
		size_t pool_index_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(pool_index); // ��ȡ��ǰλ�ö�Ӧ���ڴ�������

		constexpr size_t alloc_len = __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__;
		// ����ó��ӵ�����������Ѿ���������ȥ����һ�����Ǿʹ������ұ߸��󴦵������һ��
		//if (__MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MAllocator::pool_mem_total < type_size)
		//{
		//	// ȱ��
		//	size_t free_pool_index					= MAllocator::pool_freelist_index(type_size);
		//	size_t apply_mem_index					= free_pool_index + 1;
		//	size_t apply_mem_block_count			= 0;// һ�����ڴ�����type_size��Ҫ���ٿ����
		//	size_t apply_mem_block_index			= 0;// ���ڴ�����
		//	size_t apply_mem_block_specification	= 0;
		//	MAllocatorRep* free_mem_ptr				= nullptr;
		//	if (MAllocator::pool_mem_pool[free_pool_index] != nullptr)
		//	{
		//		goto POOL_NO_RESIDUAL_VALUE;
		//	}
		//	// ������Ȼ��ʣ�������
		//	for (apply_mem_index; apply_mem_index < __MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__; ++apply_mem_index)
		//	{
		//		if (MAllocator::pool_mem_pool[apply_mem_index] != nullptr)
		//		{
		//			free_mem_ptr = MAllocator::pool_mem_pool[apply_mem_index];
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
		// ���ܲ�ȷ�������Ҳ�ʵ��

		// ���ָõ�֮ǰδ������߸õ�֮ǰ����ķ������� 
		if (MAllocator::pool_mem_pool[pool_index] == nullptr)
		{
			// ����ս�����Ѿ����˻��ߴ�δ����
			if (MAllocator::pool_start_free_pool_ptr == MAllocator::pool_end_free_pool_ptr)
			{
				// һ������������������ һ������ս����
				MAllocator::pool_mem_pool[pool_index] = static_cast<MAllocatorRep*>
					(MAllocator::pool_apply_mem_from_sys(alloc_len * type_size * 2));

				// ��ȡս����
				MAllocator::pool_start_free_pool_ptr = MAllocator::pool_mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MAllocator::pool_end_free_pool_ptr = (MAllocatorRep*)((char*)MAllocator::pool_start_free_pool_ptr + alloc_len * type_size);

				// �����뵽���ڴ�����и�
				MAllocator::pool_mem_split
				(MAllocator::pool_mem_pool[pool_index], pool_index_specification, __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__);

				ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
				MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_mem_pool[pool_index]->next;
				MAllocator::pool_mem_total += pool_index_specification;
			}
			else// ս������ʣ��
			{
				size_t pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__(MAllocator::pool_start_free_pool_ptr, MAllocator::pool_end_free_pool_ptr);
				if (type_size <= pool_size)// ��ʣ���Ҵ���������
				{
					size_t pool_block_count = pool_size / type_size;// ���Էָ���ٸ��ڴ��
					size_t residue_mem = pool_size % type_size;// ʣ���ڴ�
					MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_start_free_pool_ptr;
					// �ָ��ڴ� ��ͷ����ָ�
					// ����ָ��
					MAllocatorRep* last_ptr = MAllocator::pool_mem_split
					(MAllocator::pool_start_free_pool_ptr, pool_index_specification, pool_block_count);
					//�����ڴ��ȥ
					ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
					MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_mem_pool[pool_index]->next;
					MAllocator::pool_mem_total += pool_index_specification;
					MAllocator::pool_start_free_pool_ptr = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_STATR_PTR_BY_LAST_ELEMENT__\
						(MAllocator::pool_end_free_pool_ptr, last_ptr, pool_index_specification);
					pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__\
						(MAllocator::pool_start_free_pool_ptr, MAllocator::pool_end_free_pool_ptr);
				}
				else if (type_size > __MUZI_ALLOCATOR_MOD_POOL_ALIGN__)// ��ʣ�൫С�������� �Ҵ�����Сֵ
				{
					//�����Ƚ�ʣ�������Ϸָ�޽ӵ������ڴ�λ�ã�ֱ��С����С���Ϊֹ�������ڴ���Ƭ��Ȼ����Ϊ�����ڴ�����������䣬������ս����
					while (pool_size != 0)
					{
						size_t free_pool_2_mem_pool_index = MAllocator::pool_freelist_index(pool_size);
						size_t free_pool_index_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(free_pool_2_mem_pool_index);
						size_t free_pool_block_count = pool_size / free_pool_index_specification;
						MAllocatorRep* last_ptr = MAllocator::pool_mem_split
						(MAllocator::pool_mem_pool[free_pool_2_mem_pool_index], free_pool_index_specification, free_pool_block_count);

						if (MAllocator::pool_mem_pool[free_pool_2_mem_pool_index] != nullptr)
						{
							// ��ʣ��ֵ�ӵ�ƥ��λ��ȥ
							last_ptr->next = MAllocator::pool_mem_pool[free_pool_2_mem_pool_index];
						}

						MAllocator::pool_mem_pool[free_pool_2_mem_pool_index] = MAllocator::pool_start_free_pool_ptr;
						int difference_value = ((char*)MAllocator::pool_end_free_pool_ptr - (char*)last_ptr) / 8;

						if (difference_value != 0)// ��ʣ����Ƭ������ս����
						{
							MAllocator::pool_start_free_pool_ptr = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_STATR_PTR_BY_LAST_ELEMENT__\
								(MAllocator::pool_end_free_pool_ptr, last_ptr, free_pool_index_specification);
							pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__\
								(MAllocator::pool_start_free_pool_ptr, MAllocator::pool_end_free_pool_ptr);
							if (pool_size < __MUZI_ALLOCATOR_MOD_POOL_MIN_SPECIFICATION__)
							{
								goto POOL_SIZE_TO_SET_0;
							}
						}
						else// ���¹�Լ
						{
						POOL_SIZE_TO_SET_0:
							MAllocator::pool_start_free_pool_ptr = nullptr;
							MAllocator::pool_end_free_pool_ptr = nullptr;
							pool_size = 0;
						}
					}
					// ��������Ϊ�������ڴ�
					MAllocator::pool_mem_pool[pool_index] = (MAllocatorRep*)MAllocator::pool_apply_mem_from_sys\
						(type_size + __MUZI_ALLOCATOR_MOD_POOL_ROUNDUP__(MAllocator::pool_mem_from_sys_total));
					if (MAllocator::pool_mem_pool[pool_index] != nullptr && !MAllocator::is_error_sys_mem((void**)&MAllocator::pool_mem_pool[pool_index]))//����ɹ�
					{
						size_t mem_index = MAllocator::pool_freelist_index(type_size);
						size_t mem_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(mem_index);
						MAllocatorRep* end_ptr = MAllocator::pool_mem_split(MAllocator::pool_mem_pool[pool_index], mem_specification, type_size / mem_specification);

						ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
						pool_mem_pool[pool_index] = pool_mem_pool[pool_index]->next;
						MAllocator::pool_start_free_pool_ptr = (MAllocatorRep*)((char*)end_ptr + mem_specification);
						MAllocator::pool_end_free_pool_ptr = (MAllocatorRep*)((char*)MAllocator::pool_start_free_pool_ptr + __MUZI_ALLOCATOR_MOD_POOL_ROUNDUP__(MAllocator::pool_mem_from_sys_total));
						goto POOL_ALLOCATE_END;
					}
					ret_mem_ptr = nullptr;
				}
				else// ��ʣ�� ���Ѿ�С����Сֵ��
				{
					// ����ʼָ���βָ����Ϊnullptr�����µ��ú���ʹ�����������֧
					MAllocator::pool_start_free_pool_ptr = nullptr;
					MAllocator::pool_end_free_pool_ptr = nullptr;
					return MAllocator::pool_allocate(type_size);
				}
			}
		}
		else// ����Ѿ����������Ȼ�п���
		{
			ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
			MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_mem_pool[pool_index]->next;
			MAllocator::pool_mem_total += __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(pool_index);
		}
	POOL_ALLOCATE_END:
		if (ret_mem_ptr != nullptr)
			ret_mem_ptr->data[0] = 0;
		return static_cast<void*>(ret_mem_ptr);
	}
	void MAllocator::pool_deallocate(void** ptr, size_t mem_size)
	{
		size_t mem_index = MAllocator::pool_freelist_index(mem_size);
		MAllocatorRep* tmp_ptr = MAllocator::pool_mem_pool[mem_index];
		MAllocatorRep* giveback_ptr = static_cast<MAllocatorRep*>(*ptr);
		// ���黹�������ڶ���   
		giveback_ptr->next = tmp_ptr;
		giveback_ptr->data[0] |= __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;
		MAllocator::pool_mem_pool[mem_index] = giveback_ptr;
		MAllocator::pool_mem_total -= mem_size;
		*ptr = nullptr;// ��ָ��ָ��� ʹ����delete�󲻻ᱻԽȨ����
	}
	void* MAllocator::pool_reallocate()
	{
		return nullptr;
	}

#endif // __MUZI_ALLOCATOR_MOD_POOL__


}
