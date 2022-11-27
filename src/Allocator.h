#pragma once

#ifndef __MUZI_ALLOCATOR_H__
#define __MUZI_ALLOCATOR_H__

#include<stdlib.h>
#include<memory>
#include<new>

//编译模式选择，即选择需要数据结构
#define __MUZI_ALLOCATOR_MOD_POOL__ 
#define __MUZI_ALLOCATOR_MOD_BITMAP__ 
#define __MUZI_ALLOCATOR_MOD_FIXED__ 

#define __MUZI_ALLOCATOR_DEFAULT__ __MUZI_ALLOCATOR_MOD_POOL__ 

//每次申请大块内存的长度 20为经验值
#define __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__ 20

// POOL模式下的一些预设宏定义
#ifdef __MUZI_ALLOCATOR_MOD_POOL__

#define __MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__ 16 

#endif // __MUZI_ALLOCATOR_MOD_POOL__



namespace MUZI
{
	class MAllocator
	{
	public:// 公用数据
		static size_t object_num;
	public:// 公用构造函数
		MAllocator()
		{
			MAllocator::object_num += 1;
		}
#ifdef __MUZI_ALLOCATOR_MOD_POOL__ // 不同模式下的不同数据结构和对应算法
	public://数据模型
		union MAllocatorRep
		{
			union MAllocatorRep* next;
			char data[1];
		};
	public://数据结构
		// 不同规格的申请量所规定的数组序列， 内部的指针维持着一个单向链表
		static union MAllocatorRep* mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__];
		static size_t mem_total;
		static union MAllocatorRep* start_free_pool_ptr;// 控制战备池头部元素的地址
		static union MAllocatorRep* end_free_pool_ptr;// 控制战备池尾部元素的地址

	private:
		// 追加量 调整数据大小上界，并且调整数据为8的边界
		inline size_t RoundUp(int denominator)
		{
			return this->mem_total / denominator;
		}
	public:
		template<typename T>
		static T* allocate(size_t type_size)
		{
			size_t pool_index = type_size / 8 - 1; // 由大小（八的倍数）获取其坐标
			size_t pool_index_specification = (pool_index + 1) * 8; // 获取当前位置对应的内存申请规格

			size_t alloc_len = type_size * __MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__ * __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__;

			if (MAllocator::mem_total == 0)
			{
				// 一半用以正常申请内容 一半用以战备池
				MAllocator::mem_pool[pool_index] =
					static_cast<MAllocatorRep*>
					(::operator new(alloc_len * 2));

				// 获取战备池
				MAllocator::start_free_pool_ptr = MAllocator::mem_pool[pool_index] + (alloc_len + 1) * type_size;
				MAllocator::end_free_pool_ptr = MAllocator::start_free_pool_ptr + alloc_len * type_size;
				
				// 记录申请总量
				MAllocator::mem_total += alloc_len;

				// 将申请到的内存进行切割
				MAllocatorRep* list_next_ptr =
					(MAllocatorRep*)((char*)MAllocator::start_free_pool_ptr - pool_index_specification);
				list_next_ptr->next = nullptr;
				MAllocatorRep* list_next_ptr_temp = list_next_ptr;

				// 构建空闲单向链表来切割内存
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