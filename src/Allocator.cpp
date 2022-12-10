#include"Allocator.h"

namespace MUZI {

	MAllocator::MAllocator()
	{
		// �������ü��� ȷ�����г����˳��� �ڴ�õ��ͷ�
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

	// ��Լ�ڴ��С
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
		// ��������ڴ���з��䣨��ͷ��ʼ��
		MAllocatorRep* list_next_ptr		= start_ptr;
		MAllocatorRep* list_next_ptr_temp	= list_next_ptr;// ��ʱָ��
		// �������е����������и��ڴ�
		for (int i = 0; i < mem_block_count; ++i)
		{
			// ���ڴ水����С����
			list_next_ptr_temp		= (MAllocatorRep*)((char*)list_next_ptr + mem_specification);
			list_next_ptr->next		= list_next_ptr_temp;
			list_next_ptr->data[0]  |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
			list_next_ptr			= list_next_ptr_temp;
		}
		// β�����˫��ֹ��
		list_next_ptr->next			= nullptr;
		list_next_ptr->data[0]		|= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
		return list_next_ptr;

	}

	void* MAllocator::pool_allocate(size_t type_size)
	{
		// �����Ͻ� ��ת����mallocȥ���� ��ʱmalloc������Ŀ���������ݱ����Ѿ���С
		if (type_size > (size_t)__MUZI_ALLOCAOTR_MOD_POOL_MAX_SPECIFICATION__)
		{
			return ::operator new(type_size);
		}

		MAllocatorRep* ret_mem_ptr			= nullptr;
		size_t pool_index					= MAllocator::pool_freelist_index(type_size); // �ɴ�С���˵ı�������ȡ������
		size_t pool_index_specification		= __MUZI_ALLOCAOTR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(pool_index); // ��ȡ��ǰλ�ö�Ӧ���ڴ�������

		constexpr size_t alloc_len			= __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__;
		// ����ó��ӵ�����������Ѿ���������ȥ����һ�����Ǿʹ������ұ߸��󴦵������һ��
		if (__MUZI_ALLOCAOTR_MOD_POOL_APPLY_MEM_MAX_SIZE__ - MAllocator::pool_mem_total < type_size)
		{
			// ȱ��
			size_t free_pool_index			= MAllocator::pool_freelist_index(type_size);
			size_t apply_mem_index			= free_pool_index + 1;
			size_t apply_mem_block_count	= 0;// һ�����ڴ�����type_size��Ҫ���ٿ����
			size_t apply_mem_block_index	= 0;// ���ڴ�����
			if (MAllocator::pool_mem_pool[free_pool_index] != nullptr)
			{
				goto POOL_NO_RESIDUAL_VALUE;
			}
			// ������Ȼ��ʣ�������
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

		// ���ָõ�֮ǰδ������߸õ�֮ǰ����ķ������� 
		if (MAllocator::pool_mem_pool[pool_index] == nullptr)
		{
			// ����ս�����Ѿ����˻��ߴ�δ����
			if (MAllocator::pool_start_free_pool_ptr == MAllocator::pool_end_free_pool_ptr)
			{
				// һ������������������ һ������ս����
				MAllocator::pool_mem_pool[pool_index] =
					static_cast<MAllocatorRep*>
					(::operator new(alloc_len * type_size * 2));

				// ��ȡս����
				MAllocator::pool_start_free_pool_ptr	= MAllocator::pool_mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MAllocator::pool_end_free_pool_ptr		= MAllocator::pool_start_free_pool_ptr + alloc_len * type_size;

				// ��¼��������
				MAllocator::pool_mem_from_sys_total		+= alloc_len * type_size * 2;

				// �����뵽���ڴ�����и�
				MAllocator::pool_mem_split
				(MAllocator::pool_mem_pool[pool_index], pool_index_specification, __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__);

				ret_mem_ptr								= MAllocator::pool_mem_pool[pool_index];
				MAllocator::pool_mem_pool[pool_index]	= MAllocator::pool_mem_pool[pool_index]->next;
				MAllocator::pool_mem_total				+= pool_index_specification;
			}
			else// ս������ʣ��
			{
				size_t pool_size = /*����ʣ���� �����Զ�Ӧbit-�ֽڻ���*/
					(MAllocator::pool_end_free_pool_ptr - MAllocator::pool_start_free_pool_ptr) / 8;
				if (type_size <= pool_size)// ��ʣ���Ҵ���������
				{
					size_t pool_block_count					= pool_size / type_size;// ���Էָ���ٸ��ڴ��
					size_t residue_mem						= pool_size % type_size;// ʣ���ڴ�
					MAllocator::pool_mem_pool[pool_index]	= MAllocator::pool_start_free_pool_ptr;
					// �ָ��ڴ� ��ͷ����ָ�

					MAllocator::pool_mem_split
					(MAllocator::pool_start_free_pool_ptr, pool_index_specification, pool_block_count);

					ret_mem_ptr								= MAllocator::pool_mem_pool[pool_index];
					MAllocator::pool_mem_pool[pool_index]	= MAllocator::pool_mem_pool[pool_index]->next;
					MAllocator::pool_mem_total				+= pool_index_specification;
				}
				else if(type_size > __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__)// ��ʣ�൫С�������� �Ҵ�����Сֵ
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

					if (difference_value != 0)// ������ս����
					{
						MAllocator::pool_start_free_pool_ptr = MAllocator::pool_end_free_pool_ptr + free_pool_index_specification - free_pool_index_specification * __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__;
					}
					else// ���¹�Լ
					{
						MAllocator::pool_start_free_pool_ptr	= nullptr;
						MAllocator::pool_end_free_pool_ptr		= nullptr;
					}
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
		// ���黹�������ڶ���   
		giveback_ptr->next						= tmp_ptr;
		giveback_ptr->data[0]					|= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;
		MAllocator::pool_mem_pool[mem_index]	= giveback_ptr;
		*ptr = nullptr;// ��ָ��ָ��� ʹ����delete�󲻻ᱻԽȨ����
	}
	void* MAllocator::pool_reallocate()
	{
		return nullptr;
	}

#endif // __MUZI_ALLOCATOR_MOD_POOL__


};
