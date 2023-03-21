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
		return (&mem_ptr == (void*)MPoolAllocator::sys_memory_block)?(delete *mem_ptr, *mem_ptr = nullptr, true):false;
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

#endif // __MUZI_ALLOCATOR_MOD_POOL__

#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
	// Chunk����ģ�飨��ײ㣩
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

		// ����Ƕ��ʽָ��
		unsigned char i = 0;
		unsigned char* p = this->p_data;
		for (; i != block_num; p += block_size)
			*p = ++i;
		// ��ÿ�������ڴ����ͷ��ǰһ���ֽ��м�¼�ڴ���������
	}
	void MLOKIAllocator::MFixedAllocator::MChunk::Release()
	{
		delete[] this->p_data;// �ͷ��Լ�
		p_data = nullptr;// ������� 
	}
	void* MLOKIAllocator::MFixedAllocator::MChunk::Allocate(size_t block_size)
	{
		if (!this->blocks_available)
			return nullptr;
		// ��ת����ǰ�ɷ����ȥ���ڴ��
		unsigned char* p_result = this->p_data + (this->first_available_block * block_size);
		this->first_available_block = *p_result;
		--blocks_available;// ���Ŀ�����Ŀ
		
		return p_result;
	}
	void MLOKIAllocator::MFixedAllocator::MChunk::Deallocate(void* p, size_t block_num)
	{
		// �ú������ϲ㺯��ȷ�������ڸ�chunk���ٵ��øú������л�����Դ
		unsigned char* to_release = static_cast<unsigned char*>(p);

		*to_release = this->first_available_block;
		this->first_available_block = static_cast<unsigned char>(to_release - this->p_data) / block_num;
		++this->blocks_available;
	}

	// FixedAllocator(�м��)
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
			// Ŀǰû�б궨chunk���߸�chunk�Ѿ�û�п���������
			for (auto chunk : this->chunks)
			{
				if (chunk.blocks_available > 0)
				{
					// �п�������
					this->alloc_chunk = &chunk;
					goto __MAllocator_MFixedAllocator_Allocate_Ret__;
				}
			}
			this->chunks.emplace_back(MChunk());
			MChunk& newChunk = this->chunks.back();// ָ��ĩ��chunk
			newChunk.Init(this->block_size, this->block_num);// ��������
			this->alloc_chunk = &newChunk;// �궨���Ժ���ø�����allocate������ȡ�ڴ��
			dealloc_chunk = &this->chunks.front();// ��Ϊvector�����ڲ�������ݽ��аᶯ��ÿ�ζ���Ҫ�������ã�������Ҫ�궨ͷ��
		}
__MAllocator_MFixedAllocator_Allocate_Ret__:
		return alloc_chunk->Allocate(block_size);
		// �ڴ��ҵ�chunk���´������ڴ�����
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
	MLOKIAllocator::MFixedAllocator::MChunk* MLOKIAllocator::MFixedAllocator::VicinityFind(void *p)// �ٽ����ҷ�����LOKI����׫д
	{
		const size_t chunk_length =  this->block_size * this->block_num;
		unsigned char* p_tmp = static_cast<unsigned char*>(p);

		// ����ͬVC6��malloc���ҵķ�ʽ ͨ���׵�ַ + ���������С �ó���Ӧָ���Ƿ������ĳ��Chunk����
		MChunk* lo = this->dealloc_chunk;// ��ʶ��һ�ι黹
		MChunk* hi = this->dealloc_chunk + 1; // ��ʶ��һ�ι黹��chunk��ʶ��һ����ʶ
		MChunk* lo_bound = &this->chunks.front();// lo���ҵ��յ�
		MChunk* hi_bound = &this->chunks.back() + 1;// hi���ҵ��յ�

		// ��Ҫ˼��Ϊ���ϴλ��յĵط�������Ѱ�� �������������յ�
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
			if (lo == hi)// ֻ�е�������Ϊnullptrʱ�Ż���� 
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
			// ��Ϊ��Ҫ��ȫ�黹��chunk ����back()һ����һ�����п�
			MChunk& last_chunk = this->chunks.back();
			// ȷ���ڸ�Chunk���ڴ���Ѿ�ȫ������
			// ȷ��������ȫ���յ��ڴ���Ż�黹һ��������ϵͳ
			if (&last_chunk == dealloc_chunk)
			{
				// �����ǰdealloc��chunk�����Ѿ�ȫ���յĵڶ���
				if (this->chunks.size() > 1 &&
					(this->dealloc_chunk - 1)->blocks_available == this->block_num)
				{
					// ���
					last_chunk.Release();
					this->chunks.pop_back();
					this->alloc_chunk = this->dealloc_chunk = &this->chunks.front();
				}
				return;
			}
			if (last_chunk.blocks_available == this->block_num)
			{
				// ������ȫ���յ� chunk��ֱ���������һ��
				last_chunk.Release();
				chunks.pop_back();
				this->alloc_chunk = this->dealloc_chunk;
				// ��ʱ���յĲ��������һ����ӵ�
			}
			else
			{
				// ����û������ȫ���յ�ģ��
				std::swap(*dealloc_chunk, last_chunk);
				this->alloc_chunk = &this->chunks.back();
			}
		}
	}

	// LOKI���ϲ㹹��

	void MLOKIAllocator::fixed_init(void*)
	{
		this->max_object_size = __MUZI_ALLOCATOR_MOD_LOKI_MAX_OBJECT_SIZE__;
		this->chunk_size = __MUZI_ALLOCATOR_MOD_LOKI_CHUNK_SIZE__;
	}
	void MLOKIAllocator::fixed_delete()
	{}
	size_t MLOKIAllocator::find_matched_fixedallocate(size_t block_size)
	{
		size_t block_volume = block_size;// �����ڴ��С
		size_t applied_pos = 0;// �����ڴ���С��ƥ��λ��
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
	// BitMapVector
	MAllocator::BitMapVector::BitMapVector():p_data(nullptr),capacity(0)
	{}
	MAllocator::BitMapVector::BitMapVector(size_t capacity):p_data(nullptr)//��������ж��ٿ�
	{
		// ��ǰ����֤capacityΪ__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__�ı���
		this->setCapacity(capacity);
	}
	MAllocator::BitMapVector::BitMapVector(BitMapVector&& object) noexcept
	{
		this->p_data = object.p_data;
		object.p_data = nullptr;
		this->capacity = object.capacity;
		object.capacity = 0;
	}
	MAllocator::BitMapVector::~BitMapVector()
	{
		if (nullptr != this->p_data)
		{
			delete[] this->p_data->p_bitmap;
			this->p_data->p_bitmap = nullptr;
			this->p_data->p_start = this->p_data->p_end 
				= this->p_data->p_end_storage = this->p_data->p_bitmap = nullptr;
			delete this->p_data;
			this->p_data = nullptr;
		}
	}
	void MAllocator::BitMapVector::setCapacity(size_t capacity)
	{
		this->capacity = 0;
		if (__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__ * capacity < __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_SIZE__)
			this->capacity = capacity;
	}
	bool MAllocator::BitMapVector::isValid()
	{
		return  0 == this->capacity || nullptr == this->p_data;
	}
	bool MAllocator::BitMapVector::isNull()
	{
		if (nullptr == this->p_data)
		{
			this->bitmap_size = this->capacity / __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
			this->p_data->p_bitmap = new __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__[this->capacity + this->bitmap_size];
			this->p_data = new BitMapVectorData;
			this->p_data->p_start = this->p_data->p_bitmap + this->bitmap_size;
			this->p_data->p_end = this->p_data->p_end;
			this->p_data->p_end_storage = this->p_data->p_start + this->capacity + 1;
			memset(this->p_data->p_start, 0, this->capacity);
			memset(this->p_data->p_bitmap, 0, this->bitmap_size);
			return true;
		}
		return false;
	}
	bool MAllocator::BitMapVector::isAllDealloced()
	{
		if(this->p_data == nullptr)
			return false;
		uint64_t tmp_full_bit = static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__>(-1);
		for (size_t i = 0; i < this->bitmap_size; ++i)
		{
			if (0 != (this->p_data->p_bitmap[i] | tmp_full_bit))
			{
				return false;
			}
		}
		return true;
	}
	bool MAllocator::BitMapVector::isNoFull()
	{
		uint64_t nops = static_cast<uint64_t>(-1);
		for (size_t i = 0; i < this->bitmap_size; ++i)
		{
			if ((this->p_data->p_bitmap[i] & nops) != nops)
			{
				return true;
			}
		}
		return false;
	}
	bool MAllocator::BitMapVector::isSubPointer(void* p)
	{
		__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__* p_tmp = static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__*>(p);
		return p_tmp < this->p_data->p_end&& p_tmp >= this->p_data->p_start;
	}

	__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__* MAllocator::BitMapVector::operator[](size_t pos)
	{
		if (pos < (this->p_data->p_end - this->p_data->p_start) / __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__ && pos > 0)
		{
			return &this->p_data->p_start[pos];
		}
		return nullptr;
	}
	int MAllocator::BitMapVector::push_back()
	{
		this->isNull();
		uint64_t tmp_bit = (1 << (__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__));
		size_t no_full_bitmap = 0;
		if ((no_full_bitmap = this->find_no_full_bitmap()) == -1)
		{
			return -1;
		}
		size_t pos = 0;
		for (; pos < __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__; ++pos)
		{
			if ((tmp_bit & this->p_data->p_bitmap[no_full_bitmap]) == 0)
			{
				if (this->p_data->p_bitmap[no_full_bitmap] == 0)
				{
					// ÿ����һ���µ�ģ��ʹ��end+1
					// ÿ����һ��endǰһ��ģ�����ǰ��飬����end���µ��ѷ���ģ��
					this->p_data->p_end = this->p_data->p_start + pos + 1;
				}
				this->p_data->p_bitmap[no_full_bitmap] |= tmp_bit;
				
			}
			tmp_bit >>= 1;
		}
		return pos;
	}
	int MAllocator::BitMapVector::find_no_full_bitmap(size_t reverse)
	{
		size_t i = (this->bitmap_size - 1) & reverse;
		auto judged_fun = [&i, this]()->bool {
			return this->p_data->p_bitmap[i] != static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__>(-1); };

		if (reverse)
		{
			for (; i >= 0; i -= 1)
			{
				if (judged_fun()) return i;
			}
		}
		else
		{
			for (; i < this->bitmap_size; i += 1)
			{
				if(judged_fun()) return i;
			}
		}

		return -1;
	}
	void MAllocator::BitMapVector::pop_back()
	{
		if(this->isNull()) return;
		this->earse(this->p_data->p_end -= __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__);
		uint64_t tmp_bit = (1 << (__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__));
		size_t no_full_bitmap = 0;
		if ((no_full_bitmap = this->find_no_full_bitmap()) == -1)return;
		size_t pos = __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_BIT_SIZE__;
		for (; pos  > 0; --pos)
		{
			if ((tmp_bit & this->p_data->p_bitmap[no_full_bitmap]) == 0)
			{
				if (this->p_data->p_bitmap[no_full_bitmap] == 0)
				{
					// ÿ����һ���µ�ģ��ʹ��end+1
					// ÿ����һ��endǰһ��ģ�����ǰ��飬����end���µ��ѷ���ģ��
					this->p_data->p_end = this->p_data->p_start + pos + 1;
				}
			}
			tmp_bit >>= 1;
		}
	}
	size_t MAllocator::BitMapVector::earse(__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__ *p)
	{
		size_t distance  = (p - this->p_data->p_start) / __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
		if (distance > this->capacity)
		{
			return -1;
		}
		// �����¼�����������ͽ��
		size_t distance_remainder = distance % __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
		size_t distance_result = distance % __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_SIZE__;
		// ����Ӧλ��Ϊ0
		uint64_t tmp_bitmap = ~(1 << distance_remainder);
		this->p_data->p_bitmap[distance_result] &= tmp_bitmap;
	}
	void  MAllocator::BitMapVector::swap(BitMapVector&& object)
	{
		BitMapVectorData* p_tmp = this->p_data;
		this->p_data = object.p_data;
		object.p_data = p_tmp;
	}
	// BitMapVectors
	MAllocator::BitMapVectors::BitMapVectors():p_data(this->p_data = new BitMapVectorsData)
	{
		this->p_data->p_start = new BitMapVector[__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
		this->p_data->p_end = this->p_data->p_start;
		this->p_data->p_end_stoage = this->p_data->p_start + __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ + 1;
		this->p_data->p_free_list = new BitMapVectorsData * [__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
		this->p_data->p_mem_list = new BitMapVectorsData*[__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
		memset(this->p_data->p_free_list, reinterpret_cast<int>(nullptr), __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__);
		memset(this->p_data->p_mem_list, reinterpret_cast<int>(nullptr), __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__);
	}
	MAllocator::BitMapVectors::BitMapVectors(BitMapVectors&& object) noexcept
	{
		BitMapVectorsData* p_tmp = this->p_data;
		this->p_data = object.p_data;
		object.p_data = p_tmp;
	}
	MAllocator::BitMapVectors::~BitMapVectors()
	{
		if (this->p_data != nullptr)
		{
			delete[] this->p_data->p_start;
			delete[] this->p_data->p_mem_list;
			delete[] this->p_data->p_free_list;
			memset(this->p_data, reinterpret_cast<int>(nullptr), 5);
			delete this->p_data;
		}
	}
	bool MAllocator::BitMapVectors::isNull()
	{
	}
	int MAllocator::BitMapVectors::push_back(size_t array_size)
	{
		size_t pos = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
		if (pos > __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ || pos < 0)
		{
			// Խ�����
		}
		this->p_data->p_start[pos].setCapacity(array_size);
	}
	void MAllocator::BitMapVectors::pop_back()
	{
		
	}
	bool MAllocator::BitMapVectors::compare_by_array_size()
	{

	}
	void MAllocator::BitMapVectors::swap()
	{

	}
	void*  MAllocator::BitMapVectors::allocate()
	{
		size_t i = 0;
		for (; i < __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__; ++i)
		{
			if (this->p_data->p_start[i].isNoFull())// Ѱ�Ҳ���ȫ����� ���Ǿ�ֱ���������һ���ȥ
			{
				return static_cast<__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__*>(this->p_data->p_start[i][this->p_data->p_start[i].push_back()]);
			}
			// �������ȫ������Ǿ��´���һ��
			// this->push_back();
		}
		return nullptr;
	}
	void  MAllocator::BitMapVectors::deallocate(void* p)
	{
		size_t bitmap_vector_count = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
		for (size_t i = 0; i < bitmap_vector_count; ++i)
		{
			if (this->p_data->p_start[i].isSubPointer(p))
			{
				this->p_data->p_start[i].earse(static_cast<uint64_t*>(p));
			}
		}
	}
	void* MAllocator::bitmap_allocate()
	{

	}
	void MAllocator::bitmap_deallocate(void* p)
	{

	}
#endif // __MUZI_ALLOCATOR_MOD_BITMAP__



}
