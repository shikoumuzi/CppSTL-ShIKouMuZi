#include"MLOKIAllocator.h"
namespace MUZI {
	MLOKIAllocator::MLOKIAllocator()
	{

	}

	MLOKIAllocator::~MLOKIAllocator()
	{

	}

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
	MLOKIAllocator::MFixedAllocator::MChunk* MLOKIAllocator::MFixedAllocator::VicinityFind(void* p)// 临近查找法，由LOKI作者撰写
	{
		const size_t chunk_length = this->block_size * this->block_num;
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
}