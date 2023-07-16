#pragma once

#ifndef __MUZI_MALLOCATOR_H__
#define __MUZI_MALLOCATOR_H__

#include<stdlib.h>
#include<memory>
#include<new>
#include<atomic>
//编译模式选择，即选择需要数据结构
#define __MUZI_ALLOCATOR_MOD_SIZE__ 4
#define __MUZI_ALLOCATOR_MOD_POOL__ 0
#define __MUZI_ALLOCATOR_MOD_BITMAP__ 1
#define __MUZI_ALLOCATOR_MOD_LOKI__ 2
#define __MUZI_ALLOCATOR_MOD_ARRAY__ 3

#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
#include<vector>
#endif // __MUZI_ALLOCATOR_MOD_LOKI__





#ifdef __MUZI_ALLOCATOR_MOD_LOKI__
// 可申请的最大对象
#define __MUZI_ALLOCATOR_MOD_LOKI_MAX_OBJECT_SIZE__ 256

#define __MUZI_ALLOCATOR_MOD_LOKI_CHUNK_SIZE__ 512

#endif // __MUZI_ALLOCATOR_MOD_LOKI__

#ifdef __MUZI_ALLOCATOR_MOD_ARRAY__
#define __MUZI_ALLOCATOR_MOD_ARRAY_REDEALLOCATED_TIMES__ 5
#endif // __MUZI_ALLOCATOR_MOD_ARRAY__

#ifdef __MUZI_ALLOCATOR_MOD_BITMAP__
#define __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_SIZE__ 4194304U /*一个分配出去的数组最大长度为4k 即一页*/
#define __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_COUNT__ 32768U /*一个数组最多的元素个数*/
#define __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ 64
#define __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__ uint64_t
#define __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE_SIZE__ sizeof(__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__)
#define __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE_BIT_SIZE__ sizeof(T) * 8
#define __MUZI_ALLOCATOR_MOD_BITMAP_FIRST_ALLOCATED_ARRAY_SIZE__ 128
#define __MUZI_ALLOCATOR_MOD_BITMAP_INCREASEING_MULTIPLE__ 2
#endif // __MUZI_ALLOCATOR_MOD_BITMAP__




// 为了匹配大小端机器所创立的指针传递方式 x为操作指针，y为需要和指针相加减的值
#define __MUZI_MEM_BIG_BOARD_ITEARTOR_ADVANCE__(x, y) x - y
#define __MUZI_MEM_BIG_BOARD_ITEARTOR_BACK__(x, y) x + y
#define __MUZI_MEM_SMALL_BOARD_ITEARTOR_ADVANCE__(x, y) x + y
#define __MUZI_MEM_SMALL_BOARD_ITEARTOR_BACK__(x, y) x - y

/*
* author：shikoumuzi
* date：2022-12-01
*/

namespace MUZI
{
	class __declspec(novtable) IMAllocator
	{
	public:
		virtual void* allocate() = 0;
		virtual void* deallocate() = 0;
	};

	class MAllocator
	{
	public:
		static MAllocator* getMAllocator(void*p = nullptr)
		{
			return new MAllocator;
		}
	public:
		virtual void* allocate(size_t size)
		{
			return ::operator new(size);
		}
		virtual void deallocate(void* p, size_t size)
		{
			operator delete(p, size);
		}
	};


};




#endif // __MUZI_MALLOCATOR_H__