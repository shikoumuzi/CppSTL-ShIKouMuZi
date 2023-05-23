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
				unsigned char first_available_block;//���� ָ���������Ҫ��Ӧ������
				unsigned char blocks_available;//Ŀǰ�ܹ�Ӧ���ٸ�����, ���������ػص��趨ֵʱ�������ſ�����ʱ�黹��һ�ڴ��
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
			MChunk* alloc_chunk;// ��������һ�η���
			MChunk* dealloc_chunk;// ������һ�εĹ黹
			size_t block_size;// chunk��p_data�����С
			unsigned char block_num;// �����ڴ����Ŀ
		public:
			MFixedAllocator(size_t block_size, unsigned char block_num);
			~MFixedAllocator();
		public:
			void* Allocate();
			void* Deallocate(void* p);
			MChunk* VicinityFind(void* p);// ���ҵ���ӦChunk
			void DoDeallocate(void* p);// ִ�й黹����
		};
	private:
		std::vector<MFixedAllocator> fixedallocate_pools;// ��Ų�ͬ����FixedAllocate
		MFixedAllocator* p_last_alloc;// �������
		MFixedAllocator* p_last_dealloc;// �����յ�
		size_t chunk_size;
		size_t max_object_size;// ���������ڴ���С
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
