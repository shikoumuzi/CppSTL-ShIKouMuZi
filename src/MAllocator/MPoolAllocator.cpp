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
	// ��Լ�ڴ��С
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
	inline bool MPoolAllocator::is_error_sys_mem(void** mem_ptr)
	{
		return (&mem_ptr == (void*)MPoolAllocator::sys_memory_block) ? (delete * mem_ptr, *mem_ptr = nullptr, true) : false;
	}

	inline void* MPoolAllocator::pool_apply_mem_from_sys(size_t mem_size)
	{
		// ���ʣ�����������������������ô����ֱ�Ӵ�ϵͳ���벢��¼�������������򷵻�nullptr���ߵ������޷�����ϵͳ���룬ͬʱ�������ڴ�����ʱ��¼���Ա����ͳһ�ͷ�
		return (__MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MPoolAllocator::pool_mem_from_sys_total > mem_size) ? \
			(MPoolAllocator::pool_mem_from_sys_total += mem_size, MPoolAllocator::sys_memory_block[MPoolAllocator::pool_get_mem_array_free_index()] = (MAllocatorRep*)::operator new(mem_size)) : nullptr;
	}

	void* MPoolAllocator::pool_allocate(size_t type_size)
	{
		// �����Ͻ� ��ת����mallocȥ���� ��ʱmalloc������Ŀ���������ݱ����Ѿ���С
		if (type_size > (size_t)__MUZI_ALLOCATOR_MOD_POOL_MAX_SPECIFICATION__)
		{
			return ::operator new(type_size);
		}

		MAllocatorRep* ret_mem_ptr = nullptr;
		size_t pool_index = MPoolAllocator::pool_freelist_index(type_size); // �ɴ�С���˵ı�������ȡ������
		size_t pool_index_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(pool_index); // ��ȡ��ǰλ�ö�Ӧ���ڴ�������

		constexpr size_t alloc_len = __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__;
		// ����ó��ӵ�����������Ѿ���������ȥ����һ�����Ǿʹ������ұ߸��󴦵������һ��
		//if (__MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MPoolAllocator::pool_mem_total < type_size)
		//{
		//	// ȱ��
		//	size_t free_pool_index					= MPoolAllocator::pool_freelist_index(type_size);
		//	size_t apply_mem_index					= free_pool_index + 1;
		//	size_t apply_mem_block_count			= 0;// һ�����ڴ�����type_size��Ҫ���ٿ����
		//	size_t apply_mem_block_index			= 0;// ���ڴ�����
		//	size_t apply_mem_block_specification	= 0;
		//	MAllocatorRep* free_mem_ptr				= nullptr;
		//	if (MPoolAllocator::pool_mem_pool[free_pool_index] != nullptr)
		//	{
		//		goto POOL_NO_RESIDUAL_VALUE;
		//	}
		//	// ������Ȼ��ʣ�������
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
		// ���ܲ�ȷ�������Ҳ�ʵ��

		// ���ָõ�֮ǰδ������߸õ�֮ǰ����ķ������� 
		if (MPoolAllocator::pool_mem_pool[pool_index] == nullptr)
		{
			// ����ս�����Ѿ����˻��ߴ�δ����
			if (MPoolAllocator::pool_start_free_pool_ptr == MPoolAllocator::pool_end_free_pool_ptr)
			{
				// һ������������������ һ������ս����
				MPoolAllocator::pool_mem_pool[pool_index] = static_cast<MAllocatorRep*>
					(MPoolAllocator::pool_apply_mem_from_sys(alloc_len * type_size * 2));

				// ��ȡս����
				MPoolAllocator::pool_start_free_pool_ptr = MPoolAllocator::pool_mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MPoolAllocator::pool_end_free_pool_ptr = (MAllocatorRep*)((char*)MPoolAllocator::pool_start_free_pool_ptr + alloc_len * type_size);

				// �����뵽���ڴ�����и�
				MPoolAllocator::pool_mem_split
				(MPoolAllocator::pool_mem_pool[pool_index], pool_index_specification, __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__);

				ret_mem_ptr = MPoolAllocator::pool_mem_pool[pool_index];
				MPoolAllocator::pool_mem_pool[pool_index] = MPoolAllocator::pool_mem_pool[pool_index]->next;
				MPoolAllocator::pool_mem_total += pool_index_specification;
			}
			else// ս������ʣ��
			{
				size_t pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__(MPoolAllocator::pool_start_free_pool_ptr, MPoolAllocator::pool_end_free_pool_ptr);
				if (type_size <= pool_size)// ��ʣ���Ҵ���������
				{
					size_t pool_block_count = pool_size / type_size;// ���Էָ���ٸ��ڴ��
					size_t residue_mem = pool_size % type_size;// ʣ���ڴ�
					MPoolAllocator::pool_mem_pool[pool_index] = MPoolAllocator::pool_start_free_pool_ptr;
					// �ָ��ڴ� ��ͷ����ָ�
					// ����ָ��
					MAllocatorRep* last_ptr = MPoolAllocator::pool_mem_split
					(MPoolAllocator::pool_start_free_pool_ptr, pool_index_specification, pool_block_count);
					//�����ڴ��ȥ
					ret_mem_ptr = MPoolAllocator::pool_mem_pool[pool_index];
					MPoolAllocator::pool_mem_pool[pool_index] = MPoolAllocator::pool_mem_pool[pool_index]->next;
					MPoolAllocator::pool_mem_total += pool_index_specification;
					MPoolAllocator::pool_start_free_pool_ptr = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_STATR_PTR_BY_LAST_ELEMENT__\
						(MPoolAllocator::pool_end_free_pool_ptr, last_ptr, pool_index_specification);
					pool_size = __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__\
						(MPoolAllocator::pool_start_free_pool_ptr, MPoolAllocator::pool_end_free_pool_ptr);
				}
				else if (type_size > __MUZI_ALLOCATOR_MOD_POOL_ALIGN__)// ��ʣ�൫С�������� �Ҵ�����Сֵ
				{
					//�����Ƚ�ʣ�������Ϸָ�޽ӵ������ڴ�λ�ã�ֱ��С����С���Ϊֹ�������ڴ���Ƭ��Ȼ����Ϊ�����ڴ�����������䣬������ս����
					while (pool_size != 0)
					{
						size_t free_pool_2_mem_pool_index = MPoolAllocator::pool_freelist_index(pool_size);
						size_t free_pool_index_specification = __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(free_pool_2_mem_pool_index);
						size_t free_pool_block_count = pool_size / free_pool_index_specification;
						MAllocatorRep* last_ptr = MPoolAllocator::pool_mem_split
						(MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index], free_pool_index_specification, free_pool_block_count);

						if (MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index] != nullptr)
						{
							// ��ʣ��ֵ�ӵ�ƥ��λ��ȥ
							last_ptr->next = MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index];
						}

						MPoolAllocator::pool_mem_pool[free_pool_2_mem_pool_index] = MPoolAllocator::pool_start_free_pool_ptr;
						int difference_value = ((char*)MPoolAllocator::pool_end_free_pool_ptr - (char*)last_ptr) / 8;

						if (difference_value != 0)// ��ʣ����Ƭ������ս����
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
						else// ���¹�Լ
						{
						POOL_SIZE_TO_SET_0:
							MPoolAllocator::pool_start_free_pool_ptr = nullptr;
							MPoolAllocator::pool_end_free_pool_ptr = nullptr;
							pool_size = 0;
						}
					}
					// ��������Ϊ�������ڴ�
					MPoolAllocator::pool_mem_pool[pool_index] = (MAllocatorRep*)MPoolAllocator::pool_apply_mem_from_sys\
						(type_size + __MUZI_ALLOCATOR_MOD_POOL_ROUNDUP__(MPoolAllocator::pool_mem_from_sys_total));
					if (MPoolAllocator::pool_mem_pool[pool_index] != nullptr && !MPoolAllocator::is_error_sys_mem((void**)&MPoolAllocator::pool_mem_pool[pool_index]))//����ɹ�
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
				else// ��ʣ�� ���Ѿ�С����Сֵ��
				{
					// ����ʼָ���βָ����Ϊnullptr�����µ��ú���ʹ�����������֧
					MPoolAllocator::pool_start_free_pool_ptr = nullptr;
					MPoolAllocator::pool_end_free_pool_ptr = nullptr;
					return MPoolAllocator::pool_allocate(type_size);
				}
			}
		}
		else// ����Ѿ����������Ȼ�п���
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
		// ���黹�������ڶ���   
		giveback_ptr->next = tmp_ptr;
		giveback_ptr->data[0] |= __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__;
		MPoolAllocator::pool_mem_pool[mem_index] = giveback_ptr;
		MPoolAllocator::pool_mem_total -= mem_size;
		ptr = nullptr;// ��ָ��ָ��� ʹ����delete�󲻻ᱻԽȨ����
	}
	void* MPoolAllocator::pool_reallocate()
	{
		return nullptr;
	}
}