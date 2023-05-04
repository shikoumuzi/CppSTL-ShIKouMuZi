#include"MLOKIAllocator.h"
namespace MUZI {
	MLOKIAllocator::MLOKIAllocator()
	{

	}

	MLOKIAllocator::~MLOKIAllocator()
	{

	}

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
	MLOKIAllocator::MFixedAllocator::MChunk* MLOKIAllocator::MFixedAllocator::VicinityFind(void* p)// �ٽ����ҷ�����LOKI����׫д
	{
		const size_t chunk_length = this->block_size * this->block_num;
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
}