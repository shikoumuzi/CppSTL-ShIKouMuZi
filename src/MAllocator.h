#pragma once

#ifndef __MUZI_MALLOCATOR_H__
#define __MUZI_MALLOCATOR_H__

#include<stdlib.h>
#include<memory>
#include<new>

//����ģʽѡ�񣬼�ѡ����Ҫ���ݽṹ
#define __MUZI_ALLOCATOR_MOD_SIZE__ 3
//#define __MUZI_ALLOCATOR_MOD_POOL__ 0
//#define __MUZI_ALLOCATOR_MOD_BITMAP__ 1
#define __MUZI_ALLOCATOR_MOD_LOKI__ 2

#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
#include<vector>
#endif // __MUZI_ALLOCATOR_MOD_LOKI__



// ÿ���������ڴ�ĳ��� 20Ϊ����ֵ
#define __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__ 20

// POOLģʽ�µ�һЩԤ��궨��
#ifdef __MUZI_ALLOCATOR_MOD_POOL__
// �������ڴ��С�Ļ�������
#define __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__ 128
// �������ڴ�Ĺ������
#define __MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__ 16
// ���ƻ����߽�
#define __MUZI_ALLOCATOR_MOD_POOL_ROUNDUP__(x) (x >> 4) /*��ϵͳ�����ڴ�����������*/
// �߽���ƴ�С�������ڴ棩
#define __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ 8
// ���Խ��ܵ���С������, ���������ֽ���Ϊ��λ
#define __MUZI_ALLOCATOR_MOD_POOL_MIN_SPECIFICATION__ 128
// ���Խ��ܵ����������, ���������ֽ���Ϊ��λ
#define __MUZI_ALLOCATOR_MOD_POOL_MAX_SPECIFICATION__ 128
// �ڴ�ı߽�
#define __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__ 3
// ȫ�ֿ����������ڴ��� 2gb
#define __MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__  1024 * 1024 * 1024 * 2 
// ��ȡ�ڴ��ڵ����߽��Ĺ���С
#define __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(x) (x + 1) * __MUZI_ALLOCATOR_MOD_POOL_ALIGN__
// ��ȡ�ָ��ڴ���ͷָ��
#define __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_STATR_PTR_BY_LAST_ELEMENT__(end_ptr, last_ptr, mem_specification)\
		 (MAllocatorRep*)((char*)end_ptr - (int)((char*)end_ptr - ((char*)last_ptr + mem_specification)))
// ��ȡս���ش�С(�ֽ�Ϊ��λ)
#define __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__(start_ptr, end_ptr) ((char*)end_ptr - (char*)start_ptr) / 8

#endif // __MUZI_ALLOCATOR_MOD_POOL__

#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
// �������������
#define __MUZI_ALLOCATOR_MOD_LOKI_MAX_OBJECT_SIZE__ 256

#define __MUZI_ALLOCATOR_MOD_LOKI_CHUNK_SIZE__ 512

#endif // __MUZI_ALLOCATOR_MOD_LOKI__


// Ϊ��ƥ���С�˻�����������ָ�봫�ݷ�ʽ xΪ����ָ�룬yΪ��Ҫ��ָ����Ӽ���ֵ
#define __MUZI_MEM_BIG_BOARD_ITEARTOR_ADVANCE__(x, y) x - y
#define __MUZI_MEM_BIG_BOARD_ITEARTOR_BACK__(x, y) x + y
#define __MUZI_MEM_SMALL_BOARD_ITEARTOR_ADVANCE__(x, y) x + y
#define __MUZI_MEM_SMALL_BOARD_ITEARTOR_BACK__(x, y) x - y

/*
* author��shikoumuzi
* date��2022-12-01
*/

namespace MUZI
{
	class MAllocator
	{
	public:// ��������
		static size_t object_num;
		// �涨ÿ��ģʽ����Ҫ����һ����ʼ���������˴�
		using MemoryCtrlFunction = void(*)(void*);
		static MemoryCtrlFunction mcf[__MUZI_ALLOCATOR_MOD_SIZE__];
		static void* mcf_arg[__MUZI_ALLOCATOR_MOD_SIZE__];
		// �涨ÿ��ģʽ����Ҫ����һ�����������˴�
		using clearMemoryFunction = void(*)(void*);
		static clearMemoryFunction cmf[__MUZI_ALLOCATOR_MOD_SIZE__];
		static void* cmf_arg[__MUZI_ALLOCATOR_MOD_SIZE__];

	public:// ���ù��캯��
		MAllocator();
		~MAllocator();
		static void atexitDestruct();
#ifdef __MUZI_ALLOCATOR_MOD_POOL__ // ��ͬģʽ�µĲ�ͬ���ݽṹ�Ͷ�Ӧ�㷨
	public://����ģ��
		union MAllocatorRep
		{
			union MAllocatorRep* next;//ǰ�ĸ��ֽ�Ϊָ��
			uint32_t data[1];//���ĸ��ֽ�Ϊ�ڴ�߽��ʶ��
		};

	public://���ݽṹ
		// ��ͬ�������������涨���������У� �ڲ���ָ��ά����һ����������
		static union MAllocatorRep* pool_mem_pool[__MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__];
		static size_t pool_mem_total;// �����ȥ���ڴ�����
		static size_t pool_mem_from_sys_total;// ��ϵͳ������ڴ�����
		static union MAllocatorRep* pool_start_free_pool_ptr;// ����ս����ͷ��Ԫ�صĵ�ַ
		static union MAllocatorRep* pool_end_free_pool_ptr;// ����ս����β��Ԫ�صĵ�ַ -> ��ս���ص�ĩ��ַ
		static MAllocatorRep** sys_memory_block;

	private:
		static void pool_init(void*);
		static void pool_delete(void*);

	private:
		// ׷���� �������ݴ�С�Ͻ磬���ҵ�������Ϊ8�ı߽�
		static size_t pool_RoundUp(size_t bytes);// ���������ڴ�߽�
		static size_t pool_freelist_index(size_t bytes);// ��ȡ��Ӧfreelist�������±�
		static MAllocatorRep* pool_mem_split(MAllocatorRep* start_ptr, size_t mem_specification, size_t mem_block_count);// �ڴ��Ƭ����, ����βԪ�ص�ָ��
		static inline size_t pool_get_mem_array_free_index();
		static void* pool_apply_mem_from_sys(size_t mem_size);// �����ϵͳ�����ÿһ��Ԫ��
		static bool is_error_sys_mem(void** mem_ptr);//��鲻��nullptr�Ĵ�������ڴ� ��sys_memory_block�ĵ�һ��Ԫ�ص�ַ

	public:
		static void* pool_allocate(size_t type_size);// ��������ڴ�
		static void pool_deallocate(void** ptr, size_t mem_size);// �����ڴ�, ��������ô���ָ���ַ�ķ�ʽ����ԭָ���ַָ����Ա�֤����ԽȨ����
		static void* pool_reallocate();// �ӳ�ԭ��������ڴ�ռ�
		static void* pool_chunk_allocate();// ����ڴ��������
		static void* pool_refill();// ��ֵս����
		static bool pool_is_possible_mem_board(void* p);//�鿴��ǰָ����ָ���һ���ֽ��Ƿ�����ڴ�߽�
#endif // __MUZI_ALLOCATOR_MOD_POOL__

#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
		// �������ͨPOOL ������������������ķ�ʽ�����ڴ��
		// �ŵ��ǿ�����ʱ���ڴ���Դ�黹������ϵͳ���򵥾������ӻ��黹����
		// 
		class MFixedAllocator
		{
		public:
			friend class MAllocator;
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
			void* Deallocate(void *p);
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
		void fixed_dellocate(void* p);

#endif // __MUZI_ALLOCATOR_MOD_LOKI__


	};

};




#endif // __MUZI_MALLOCATOR_H__