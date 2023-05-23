#ifndef __MUZI_MLOKIALLOCATOR_H__
#define __MUZI_MLOKIALLOCATOR_H__
#include"MAllocator.h"
#include<vector>
namespace MUZI {
	class MLOKIAllocator :public MAllocator
	{
	public:
		static MAllocator* getMAllocator(void* p = nullptr)
		{
			return dynamic_cast<MAllocator*>(new MLOKIAllocator);
		}
	public:
		MLOKIAllocator();
		~MLOKIAllocator();
	public:
		class MFixedAllocator
		{
		public:
			friend class MLOKIAllocator;
		public:
			class MChunk
			{
			public:
				friend class MFixedAllocator;
			private:
				unsigned char* p_data;
				unsigned char first_available_block;//索引 指向接下来所要供应的区块
				unsigned char blocks_available;//目前能供应多少个区块, 当该数字重回到设定值时，代表着可以随时归还这一内存块
			public:
				MChunk();
				~MChunk();
			public:
				void Init(size_t block_size, unsigned char block_num);
				void Reset(size_t block_size, unsigned char block_num);
				void Release();
				void* Allocate(size_t block_size);
				void Deallocate(void* p, size_t block_num);
			};
		private:
			std::vector<MChunk> chunks;
			MChunk* alloc_chunk;// 标出最近的一次分配
			MChunk* dealloc_chunk;// 标出最近一次的归还
			size_t block_size;// chunk的p_data申请大小
			unsigned char block_num;// 单个内存块数目
		public:
			MFixedAllocator(size_t block_size, unsigned char block_num);
			~MFixedAllocator();
		public:
			void* Allocate();
			void* Deallocate(void* p);
			MChunk* VicinityFind(void* p);// 查找到对应Chunk
			void DoDeallocate(void* p);// 执行归还操作
		};
	private:
		std::vector<MFixedAllocator> fixedallocate_pools;// 存放不同规格的FixedAllocate
		MFixedAllocator* p_last_alloc;// 最后分配的
		MFixedAllocator* p_last_dealloc;// 最后回收的
		size_t chunk_size;
		size_t max_object_size;// 最大可申请内存块大小
	private:
		size_t find_matched_fixedallocate(size_t block_size);
	public:
		void fixed_init(void*);
		void fixed_delete();
		void* fixed_allocate(size_t block_size);
		void fixed_deallocate(void* p);

		void* allocate(size_t size) override
		{
			return this->fixed_allocate(size);
		}
		void deallocate(void* p, size_t size = 0) override
		{
			fixed_deallocate(p);
		}

	};
}
#endif // !__MUZI_MLOKIALLOCATOR_H__
