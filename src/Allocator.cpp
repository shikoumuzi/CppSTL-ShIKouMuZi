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
		// �����Ͻ� ��ת����mallocȥ���� ��ʱmalloc������Ŀ���������ݱ����Ѿ���С
		if (type_size > (size_t)__MUZI_ALLOCAOTR_MOD_POOL_MAX_SPECIFICATION__)
		{
			return ::operator new(type_size);
		}

		MAllocatorRep* ret_mem_ptr = nullptr;
		size_t pool_index = MAllocator::pool_freelist_index(type_size); // �ɴ�С���˵ı�������ȡ������
		size_t pool_index_specification = (pool_index + 1) * __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__; // ��ȡ��ǰλ�ö�Ӧ���ڴ�������

		constexpr size_t alloc_len = __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__;

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
				MAllocator::pool_start_free_pool_ptr = MAllocator::pool_mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MAllocator::pool_end_free_pool_ptr = MAllocator::pool_start_free_pool_ptr + alloc_len * type_size;

				// ��¼��������
				MAllocator::pool_mem_total += alloc_len;

				// �����뵽���ڴ�����и�(��β����ʼ�����и�)
				MAllocatorRep* list_next_ptr =
					(MAllocatorRep*)((char*)MAllocator::pool_start_free_pool_ptr - pool_index_specification);
				list_next_ptr->next = nullptr;
				list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
				MAllocatorRep* list_next_ptr_temp = list_next_ptr;

				// �������е����������и��ڴ�
				for (int i = 0; i < __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__; ++i)
				{
					list_next_ptr =
						(MAllocatorRep*)((char*)list_next_ptr - pool_index_specification);
					list_next_ptr->next = list_next_ptr_temp;
					list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
					list_next_ptr_temp = list_next_ptr;
				}

				ret_mem_ptr = MAllocator::pool_mem_pool[pool_index];
				MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_mem_pool[pool_index]->next;
			}
			else// ս������ʣ��
			{
				size_t pool_size = 
					(MAllocator::pool_end_free_pool_ptr - MAllocator::pool_start_free_pool_ptr) / 8;
				if (type_size <= pool_size)
				{
					size_t pool_block_count = pool_size / type_size;// ���Էָ���ٸ��ڴ��
					size_t residue_mem = pool_size % type_size;// ʣ���ڴ�
					MAllocator::pool_mem_pool[pool_index] = MAllocator::pool_start_free_pool_ptr;
					// �ָ��ڴ� ��ͷ����ָ�
					MAllocatorRep* list_next_ptr =
						static_cast<MAllocatorRep*>(MAllocator::pool_start_free_pool_ptr);
					MAllocatorRep* list_next_ptr_temp = list_next_ptr;// ��ʱָ��
					for (int i = 0; i < pool_block_count; ++i)
					{
						list_next_ptr_temp =
							(MAllocatorRep*)((char*)list_next_ptr + pool_index_specification);
						list_next_ptr->next = list_next_ptr_temp;
						list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
						list_next_ptr = list_next_ptr_temp;
					}

					list_next_ptr->next = nullptr;
					list_next_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;// ����ڴ��ʶ��
				}
				else
				{

				}
			}
		}
		else
		{
			// ����Ѿ����������Ȼ�п���
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
		// ���黹�������ڶ���   
		giveback_ptr->next = tmp_ptr;
		giveback_ptr->data[0] |= __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__;
		MAllocator::pool_mem_pool[mem_index] = giveback_ptr;
		*ptr = nullptr;// ��ָ��ָ��� ʹ����delete�󲻻ᱻԽȨ����
	}
	void* MAllocator::pool_reallocate()
	{
		return nullptr;
	}

#endif // __MUZI_ALLOCATOR_MOD_POOL__


};
