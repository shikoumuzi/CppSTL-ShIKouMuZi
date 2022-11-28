#pragma once

#ifndef __MUZI_ALLOCATOR_H__
#define __MUZI_ALLOCATOR_H__

#include<stdlib.h>
#include<memory>
#include<new>

//����ģʽѡ�񣬼�ѡ����Ҫ���ݽṹ
#define __MUZI_ALLOCATOR_MOD_SIZE__ 3
#define __MUZI_ALLOCATOR_MOD_POOL__ 
#define __MUZI_ALLOCATOR_MOD_BITMAP__ 
#define __MUZI_ALLOCATOR_MOD_FIXED__ 

// ÿ���������ڴ�ĳ��� 20Ϊ����ֵ
#define __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__ 20

// POOLģʽ�µ�һЩԤ��궨��
#ifdef __MUZI_ALLOCATOR_MOD_POOL__

// �������ڴ�Ĺ������
#define __MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__ 16
// ���ƻ����߽�
#define __MUZI_ALLOCAOTR_MOD_POOL_ROUNDUP__(x) x >> 4
// �߽���ƴ�С�������ڴ棩
#define __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ 8

#endif // __MUZI_ALLOCATOR_MOD_POOL__


namespace MUZI
{
	class MAllocator
	{
	public:// ��������
		static size_t object_num;
		// �涨ÿ��ģʽ����Ҫ����һ�����������˴�
		using clearMemoryFunction = void(*)(void*);
		static clearMemoryFunction cmf[__MUZI_ALLOCATOR_MOD_SIZE__];
		static void* cmf_arg;
		static void callCMF()
		{
			for (int i = 0; i < __MUZI_ALLOCATOR_MOD_SIZE__; ++i)
			{
				MAllocator::cmf[i](MAllocator::cmf_arg);
			}
		}
	public:// ���ù��캯��
		MAllocator()
		{
			MAllocator::object_num += 1;
			//atexit(MAllocator::callCMF);
		}
#ifdef __MUZI_ALLOCATOR_MOD_POOL__ // ��ͬģʽ�µĲ�ͬ���ݽṹ�Ͷ�Ӧ�㷨
	public://����ģ��
		union MAllocatorRep
		{
			union MAllocatorRep* next;
			char data[1];
		};
	public://���ݽṹ
		// ��ͬ�������������涨���������У� �ڲ���ָ��ά����һ����������
		static union MAllocatorRep* pool_mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__];
		static size_t pool_mem_total;
		static union MAllocatorRep* pool_start_free_pool_ptr;// ����ս����ͷ��Ԫ�صĵ�ַ
		static union MAllocatorRep* pool_end_free_pool_ptr;// ����ս����β��Ԫ�صĵ�ַ

	private:
		// ׷���� �������ݴ�С�Ͻ磬���ҵ�������Ϊ8�ı߽�
		static size_t pool_RoundUp();// ���������ڴ�߽�
		static size_t pool_freelist_index(size_t bytes);// ��ȡ��Ӧfreelist�������±�
		static void* pool_mem_split();// �ڴ��Ƭ����
	public:
		static void* pool_allocate(size_t type_size);// ��������ڴ�
		static void pool_deallocate(void* ptr, size_t mem_size);// �����ڴ�
		static void* pool_reallocate();// �ӳ�ԭ��������ڴ�ռ�
		static void* pool_chunk_allocate();// ����ڴ��������
		static void* refill();// ��ֵս����

		static void* pool_allocate_array(size_t type_array_size);
		static void pool_deallocate_array(void* ptr, size_t mem_size);
#endif // __MUZI_ALLOCATOR_MOD_POOL__

	};

};




#endif // __MUZI_ALLOCATOR_H__