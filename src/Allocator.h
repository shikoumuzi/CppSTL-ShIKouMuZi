#pragma once

#ifndef __MUZI_ALLOCATOR_H__
#define __MUZI_ALLOCATOR_H__

#include<stdlib.h>
#include<memory>
#include<new>

//编译模式选择，即选择需要数据结构
#define __MUZI_ALLOCATOR_MOD_SIZE__ 3
#define __MUZI_ALLOCATOR_MOD_POOL__ 
#define __MUZI_ALLOCATOR_MOD_BITMAP__ 
#define __MUZI_ALLOCATOR_MOD_FIXED__ 

// 每次申请大块内存的长度 20为经验值
#define __MUZI_ALLOCAOTR_MEMORY_MALLOC_SIZE__ 20

// POOL模式下的一些预设宏定义
#ifdef __MUZI_ALLOCATOR_MOD_POOL__

// 可申请内存的规格数量
#define __MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__ 16
// 控制基础边界
#define __MUZI_ALLOCAOTR_MOD_POOL_ROUNDUP__(x) x >> 4
// 边界控制大小（对齐内存）
#define __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__ 8
// 可以接受的最大申请量
#define __MUZI_ALLOCAOTR_MOD_POOL_MAX_SPECIFICATION__ 128
// 内存的边界
#define __MUZI_ALLOCAOTR_MOD_POOL_MEM_BOARD_FLAG__ 3
// 全局可申请的最大内存量 2gb
#define __MUZI_ALLOCAOTR_MOD_POOL_APPLY_MEM_MAX_SIZE__  1024 * 1024 * 1024 * 2 
// 获取内存在调整边界后的规格大小
#define __MUZI_ALLOCAOTR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(x) (x + 1) * __MUZI_ALLOCAOTR_MOD_POOL_ALIGN__

#endif // __MUZI_ALLOCATOR_MOD_POOL__

// 为了匹配大小端机器所创立的指针传递方式 x为操作指针，y为需要和指针相加减的值
#define __MUZI_MEM_BIG_BOARD_ITEARTOR_ADVANCE__(x, y) x - y
#define __MUZI_MEM_BIG_BOARD_ITEARTOR_BACK__(x, y) x + y
#define __MUZI_MEM_SMALL_BOARD_ITEARTOR_ADVANCE__(x, y) x + y
#define __MUZI_MEM_SMALL_BOARD_ITEARTOR_BACK__(x, y) x - y


namespace MUZI
{
	class MAllocator
	{
	public:// 公用数据
		static size_t object_num;
		// 规定每种模式都需要传递一个清理函数到此处
		using clearMemoryFunction = void(*)(void*);
		static clearMemoryFunction cmf[__MUZI_ALLOCATOR_MOD_SIZE__];
		static void* cmf_arg[__MUZI_ALLOCATOR_MOD_SIZE__];
		static void callCMF();
		static void setClearMemoryFunction(clearMemoryFunction fun, void* arg);
	public:// 公用构造函数
		MAllocator();
		~MAllocator();
#ifdef __MUZI_ALLOCATOR_MOD_POOL__ // 不同模式下的不同数据结构和对应算法
	public://数据模型
		union MAllocatorRep
		{
			union MAllocatorRep* next;//前四个字节为指针
			uint32_t data[1];//后四个字节为内存边界标识符
		};
	public://数据结构
		// 不同规格的申请量所规定的数组序列， 内部的指针维持着一个单向链表
		static union MAllocatorRep* pool_mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__];
		static size_t pool_mem_total;
		static union MAllocatorRep* pool_start_free_pool_ptr;// 控制战备池头部元素的地址
		static union MAllocatorRep* pool_end_free_pool_ptr;// 控制战备池尾部元素的地址

	private:
		// 追加量 调整数据大小上界，并且调整数据为8的边界
		static size_t pool_RoundUp(size_t bytes);// 调整申请内存边界
		static size_t pool_freelist_index(size_t bytes);// 获取对应freelist的数组下标
		static void pool_mem_split(MAllocatorRep* start_ptr, size_t mem_specification, size_t mem_block_count);// 内存分片函数, 返回尾指针
	public:
		static void* pool_allocate(size_t type_size);// 申请分配内存
		static void pool_deallocate(void** ptr, size_t mem_size);// 回收内存, 在这里采用传入指针地址的方式，将原指针地址指向空以保证不会越权访问
		static void* pool_reallocate();// 延长原先申请的内存空间
		static void* pool_chunk_allocate();// 大块内存分配请求
		static void* pool_refill();// 充值战备池
		static bool pool_is_possible_mem_board(void* p);//查看当前指针所指向的一个字节是否包含内存边界
#endif // __MUZI_ALLOCATOR_MOD_POOL__

	};

};




#endif // __MUZI_ALLOCATOR_H__