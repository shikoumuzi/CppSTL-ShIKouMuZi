#ifndef __MUZI_MPOOLALLOCATOR_H__
#define __MUZI_MPOOLALLOCATOR_H__

#include"MAllocator.h"
namespace MUZI {
	// ÿ���������ڴ�ĳ��� 20Ϊ����ֵ
#define __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__ 20

// POOLģʽ�µ�һЩԤ��궨��
#ifdef __MUZI_MPOOLALLOCATOR_H__
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

#endif // __MUZI_MPOOLALLOCATOR_H__
	class MPoolAllocator :public MAllocator
	{
	public:
		static MAllocator* getMAllocator(void* p = nullptr)
		{
			static class MPoolAllocator __MUZI_MPOOLALLOCATOR__;
			return dynamic_cast<MAllocator*>(&__MUZI_MPOOLALLOCATOR__);
		}
	private:
		MPoolAllocator();
	public:
		~MPoolAllocator();
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
		static void* pool_apply_mem_from_sys(size_t mem_size);// ������ϵͳ�����ÿһ��Ԫ��
		static bool is_error_sys_mem(void** mem_ptr);//��鲻��nullptr�Ĵ�������ڴ� ��sys_memory_block�ĵ�һ��Ԫ�ص�ַ

	public:
		static void* pool_allocate(size_t type_size);// ��������ڴ�
		static void pool_deallocate(void* ptr, size_t mem_size);// �����ڴ�, ��������ô���ָ���ַ�ķ�ʽ����ԭָ���ַָ����Ա�֤����ԽȨ����
		static void* pool_reallocate();// �ӳ�ԭ��������ڴ�ռ�
		static void* pool_chunk_allocate();// ����ڴ��������
		static void* pool_refill();// ��ֵս����
		static bool pool_is_possible_mem_board(void* p);//�鿴��ǰָ����ָ���һ���ֽ��Ƿ�����ڴ�߽�
	public:
		void* allocate(size_t size) override
		{
			return pool_allocate(size);
		}
		void deallocate(void* p, size_t size) override
		{
			pool_deallocate(p, size);
		}
	};
}
#endif // !__MUZI_MPOOLALLOCATOR_H__