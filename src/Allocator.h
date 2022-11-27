#pragma once

#ifndef __MUZI_ALLOCATOR_H__
#define __MUZI_ALLOCATOR_H__

#include<stdlib.h>
#include<memory>
#include<new>

//����ģʽѡ�񣬼�ѡ����Ҫ���ݽṹ
#define __MUZI_ALLOCATOR_MOD_POOL__ 
#define __MUZI_ALLOCATOR_MOD_BITMAP__ 
#define __MUZI_ALLOCATOR_MOD_FIXED__ 

#define __MUZI_ALLOCATOR_DEFAULT__ __MUZI_ALLOCATOR_MOD_POOL__ 

//ÿ���������ڴ�ĳ��� 20Ϊ����ֵ
#define __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__ 20

// POOLģʽ�µ�һЩԤ��궨��
#ifdef __MUZI_ALLOCATOR_MOD_POOL__

#define __MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__ 16 

#endif // __MUZI_ALLOCATOR_MOD_POOL__



namespace MUZI
{
	class MAllocator
	{
	public:// ��������
		static size_t object_num;
	public:// ���ù��캯��
		MAllocator()
		{
			MAllocator::object_num += 1;
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
		static union MAllocatorRep* mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__];
		static size_t mem_total;
		static union MAllocatorRep* start_free_pool_ptr;// ����ս����ͷ��Ԫ�صĵ�ַ
		static union MAllocatorRep* end_free_pool_ptr;// ����ս����β��Ԫ�صĵ�ַ

	private:
		// ׷���� �������ݴ�С�Ͻ磬���ҵ�������Ϊ8�ı߽�
		inline size_t RoundUp(int denominator)
		{
			return this->mem_total / denominator;
		}
	public:
		template<typename T>
		static T* allocate(size_t type_size)
		{
			size_t pool_index = type_size / 8 - 1; // �ɴ�С���˵ı�������ȡ������
			size_t pool_index_specification = (pool_index + 1) * 8; // ��ȡ��ǰλ�ö�Ӧ���ڴ�������

			size_t alloc_len = type_size * __MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__ * __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__;

			if (MAllocator::mem_total == 0)
			{
				// һ������������������ һ������ս����
				MAllocator::mem_pool[pool_index] =
					static_cast<MAllocatorRep*>
					(::operator new(alloc_len * 2));

				// ��ȡս����
				MAllocator::start_free_pool_ptr = MAllocator::mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MAllocator::end_free_pool_ptr = MAllocator::start_free_pool_ptr + alloc_len * type_size;
				
				// ��¼��������
				MAllocator::mem_total += alloc_len;

				// �����뵽���ڴ�����и�
				MAllocatorRep* list_next_ptr =
					(MAllocatorRep*)((char*)MAllocator::start_free_pool_ptr - pool_index_specification);
				list_next_ptr->next = nullptr;
				MAllocatorRep* list_next_ptr_temp = list_next_ptr;

				// �������е����������и��ڴ�
				for (int i = 0; i < __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__ - 1; ++i)
				{
					list_next_ptr =
						(MAllocatorRep*)((char*)list_next_ptr - pool_index_specification);
					list_next_ptr->next = list_next_ptr_temp;
					list_next_ptr_temp = list_next_ptr;
				}
			}
			else
			{
				if (MAllocator::mem_pool[pool_index] != nullptr)
				{

				}
				else
				{

				}
			}

			return static_cast<T*>(MAllocator::mem_pool[pool_index]);
		}
		static void deallocate(void* ptr, size_t mem_size)
		{
			char* p = nullptr;

		}
		static void* allocate_array(size_t type_array_size)
		{
			char* p = nullptr;

			return p;
		}
		static void deallocate_array(void* ptr, size_t mem_size)
		{
			char* p = nullptr;

		}
#endif // __MUZI_ALLOCATOR_MOD_POOL__

	};

};




#endif // __MUZI_ALLOCATOR_H__