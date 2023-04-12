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



// 每次申请大块内存的长度 20为经验值
#define __MUZI_ALLOCATOR_MEMORY_MALLOC_SIZE__ 20

// POOL模式下的一些预设宏定义
#ifdef __MUZI_ALLOCATOR_MOD_POOL__
// 可申请内存大小的基础次数
#define __MUZI_ALLOCATOR_MOD_POOL_MEM_ARRAY_LENGTH__ 128
// 可申请内存的规格数量
#define __MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__ 16
// 控制基础边界
#define __MUZI_ALLOCATOR_MOD_POOL_ROUNDUP__(x) (x >> 4) /*向系统申请内存这块存在问题*/
// 边界控制大小（对齐内存）
#define __MUZI_ALLOCATOR_MOD_POOL_ALIGN__ 8
// 可以接受的最小申请量, 申请规格按照字节作为单位
#define __MUZI_ALLOCATOR_MOD_POOL_MIN_SPECIFICATION__ 128
// 可以接受的最大申请量, 申请规格按照字节作为单位
#define __MUZI_ALLOCATOR_MOD_POOL_MAX_SPECIFICATION__ 128
// 内存的边界
#define __MUZI_ALLOCATOR_MOD_POOL_MEM_BOARD_FLAG__ 3
// 全局可申请的最大内存量 2gb
#define __MUZI_ALLOCATOR_MOD_POOL_APPLY_MEM_MAX_SIZE__  1024 * 1024 * 1024 * 2 
// 获取内存在调整边界后的规格大小
#define __MUZI_ALLOCATOR_MOD_POOL_GET_SPECIFICATION_BY_INDEX__(x) (x + 1) * __MUZI_ALLOCATOR_MOD_POOL_ALIGN__
// 获取分割内存后的头指针
#define __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_STATR_PTR_BY_LAST_ELEMENT__(end_ptr, last_ptr, mem_specification)\
		 (MAllocatorRep*)((char*)end_ptr - (int)((char*)end_ptr - ((char*)last_ptr + mem_specification)))
// 获取战备池大小(字节为单位)
#define __MUZI_ALLOCATOR_MOD_POOL_GET_FREE_POOL_SIZE__(start_ptr, end_ptr) ((char*)end_ptr - (char*)start_ptr) / 8

#endif // __MUZI_ALLOCATOR_MOD_POOL__

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
	class MAllocator
	{
	public:
		virtual MAllocator* getMAllocate(void*p = nullptr)
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
#ifdef __MUZI_ALLOCATOR_MOD_POOL__ // 不同模式下的不同数据结构和对应算法

	class MPoolAllocator:public MAllocator
	{
	public:
		MAllocator* getMAllocate(void* p = nullptr)  override
		{
			static class MPoolAllocator __MUZI_MPOOLALLOCATOR__;
			return dynamic_cast<MAllocator*>(&__MUZI_MPOOLALLOCATOR__);
		}
	private:
		MPoolAllocator();
	public:
		~MPoolAllocator();
	public://数据模型
		union MAllocatorRep
		{
			union MAllocatorRep* next;//前四个字节为指针
			uint32_t data[1];//后四个字节为内存边界标识符
		};

	public://数据结构
		// 不同规格的申请量所规定的数组序列， 内部的指针维持着一个单向链表
		static union MAllocatorRep* pool_mem_pool[__MUZI_ALLOCATOR_MOD_POOL_SPECIFICATION_COUNT__];
		static size_t pool_mem_total;// 分配出去的内存总量
		static size_t pool_mem_from_sys_total;// 向系统申请的内存总量
		static union MAllocatorRep* pool_start_free_pool_ptr;// 控制战备池头部元素的地址
		static union MAllocatorRep* pool_end_free_pool_ptr;// 控制战备池尾部元素的地址 -> 改战备池的末地址
		static MAllocatorRep** sys_memory_block;

	private:
		static void pool_init(void*);
		static void pool_delete(void*);
	private:
		// 追加量 调整数据大小上界，并且调整数据为8的边界
		static size_t pool_RoundUp(size_t bytes);// 调整申请内存边界
		static size_t pool_freelist_index(size_t bytes);// 获取对应freelist的数组下标
		static MAllocatorRep* pool_mem_split(MAllocatorRep* start_ptr, size_t mem_specification, size_t mem_block_count);// 内存分片函数, 返回尾元素的指针
		static inline size_t pool_get_mem_array_free_index();
		static void* pool_apply_mem_from_sys(size_t mem_size);// 管理从系统申请的每一个元素
		static bool is_error_sys_mem(void** mem_ptr);//检查不是nullptr的错误分配内存 即sys_memory_block的第一个元素地址

	public:
		static void* pool_allocate(size_t type_size);// 申请分配内存
		static void pool_deallocate(void* ptr, size_t mem_size);// 回收内存, 在这里采用传入指针地址的方式，将原指针地址指向空以保证不会越权访问
		static void* pool_reallocate();// 延长原先申请的内存空间
		static void* pool_chunk_allocate();// 大块内存分配请求
		static void* pool_refill();// 充值战备池
		static bool pool_is_possible_mem_board(void* p);//查看当前指针所指向的一个字节是否包含内存边界
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
#endif // __MUZI_ALLOCATOR_MOD_POOL__

#ifdef __MUZI_ALLOCATOR_MOD_LOKI__

	class MLOKIAllocator:public MAllocator
	{
	public:
		MAllocator* getMAllocate(void* p = nullptr) override
		{
			return dynamic_cast<MAllocator*>(new MLOKIAllocator);
		}
	private:
		MLOKIAllocator();
	public:
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
#endif // __MUZI_ALLOCATOR_MOD_LOKI__

#ifdef __MUZI_ALLOCATOR_MOD_ARRAY__
	class MArrayAllocate: public MAllocator
	{
	public:
		class ArraryAllocateArgs
		{
		public:
			friend class MArrayAllocate;
		public:
			// ArraryAllocateArgs件
			ArraryAllocateArgs(void* p_array, size_t type_size, size_t array_length)
				:arg_array(p_array), block_size(type_size), arg_array_length(array_length) {}
		private:
			void* arg_array;
			size_t block_size;
			size_t arg_array_length;
		};
	private:
		struct ArrayMemoryRecord
		{
			unsigned char* start;
			unsigned char* end;
		};
	public:
		void array_init(void* p)
		{
			ArraryAllocateArgs* p_args = static_cast<ArraryAllocateArgs*>(p);
			this->array_data = static_cast<unsigned char*>(p_args->arg_array);
			this->array_block_size = p_args->block_size;
			this->array_data_length = p_args->arg_array_length;
			this->array_array_memory_record_head = new unsigned char[p_args->arg_array_length + 1];
			this->array_dealloc_times = __MUZI_ALLOCATOR_MOD_ARRAY_REDEALLOCATED_TIMES__;
		}
		void* array_allocate(size_t block_num);
		void array_deallocate(void* p);
		void dellocate_Rep();
	private:
		size_t array_block_size;
		unsigned char* array_data;
		size_t array_data_length;
		size_t array_dealloc_times;
		unsigned char* array_array_memory_record_head;
		unsigned char* array_bitmap;
	public:
		void* allocate(size_t size) override
		{
			return array_allocate(1);
		}
		void deallocate(void* p, size_t size = 0) override
		{
			array_deallocate(p);
		}

	};
#endif // __MUZI_ALLOCATOR_MOD_ARRAY__

#ifdef __MUZI_ALLOCATOR_MOD_BITMAP__
	// 相比起普通POOL 这里采用数组代替链表的方式保管内存块
	// 优点是可以随时将内存资源归还给操作系统、简单精简、有延缓归还能力
	// 
	template<typename T>
	concept __muzi_alloctor_mod_bitmap_data_type__ = requires(T x)
	{
		std::unsigned_integral<T>;
		sizeof(T) % 16 == 0;
	};

	template<__muzi_alloctor_mod_bitmap_data_type__ T = uint64_t>
	class MBitmapAllocate:public MAllocator
	{
	public:
		MAllocator* getMAllocator(void* p = nullptr) override
		{
			return dynamic_cast<MAllocator*>(new MBitmapAllocate);
		}
	private:
		MBitmapAllocate();
	public:
		~MBitmapAllocate();
	private:
		class BitMapVector
		{
		public:
			struct BitMapVectorData
			{
				T* p_bitmap;
				T* p_start;
				T* p_end;
				T* p_end_storage;
			};
		public:
			friend class BitMapVectors;
		public:
			BitMapVector() :p_data(nullptr), capacity(0)
			{}
			BitMapVector(size_t capacity) :p_data(nullptr)//输入的是有多少块
			{
				// 在前件保证capacity为sizeof(T)的倍数
				this->setCapacity(capacity);
			}
			BitMapVector(BitMapVector&& object) noexcept
			{
				this->p_data = object.p_data;
				object.p_data = nullptr;
				this->capacity = object.capacity;
				object.capacity = 0;
			}
			BitMapVector(const BitMapVector&) = delete;
			~BitMapVector()
			{
				if (nullptr != this->p_data)
				{
					delete[] this->p_data->p_bitmap;
					this->p_data->p_bitmap = nullptr;
					this->p_data->p_start = this->p_data->p_end
						= this->p_data->p_end_storage = this->p_data->p_bitmap = nullptr;
					delete this->p_data;
					this->p_data = nullptr;
				}
			}
		public:
			T* operator[](size_t pos)
			{
				if (pos < (this->p_data->p_end - this->p_data->p_start) / sizeof(T) && pos > 0)
				{
					return &this->p_data->p_start[pos];
				}
				return nullptr;
			}
			int push_back()// 标记最后一个能用的
			{
				this->isNull();
				uint64_t tmp_bit = (1 << (sizeof(T) * 8));
				size_t no_full_bitmap = 0;
				if ((no_full_bitmap = this->find_no_full_bitmap()) == -1)
				{
					return -1;
				}
				size_t pos = 0;
				for (; pos < (sizeof(T) * 8); ++pos)
				{
					if ((tmp_bit & this->p_data->p_bitmap[no_full_bitmap]) == 0)
					{
						if (this->p_data->p_bitmap[no_full_bitmap] == 0)
						{
							// 每开辟一个新的模块使得end+1
							// 每回收一次end前一个模块就向前检查，调整end至新的已分配模块
							this->p_data->p_end = this->p_data->p_start + pos + 1;
						}
						this->p_data->p_bitmap[no_full_bitmap] |= tmp_bit;

					}
					tmp_bit >>= 1;
				}
				return pos;
			}
			void pop_back()// 标记最后一个为已回收内存块
			{
				if (this->isNull()) return;
				this->earse(this->p_data->p_end -= sizeof(T));
				uint64_t tmp_bit = (1 << ((sizeof(T) * 8)));
				size_t no_full_bitmap = 0;
				if ((no_full_bitmap = this->find_no_full_bitmap()) == -1)return;
				size_t pos = (sizeof(T) * 8);
				for (; pos > 0; --pos)
				{
					if ((tmp_bit & this->p_data->p_bitmap[no_full_bitmap]) == 0)
					{
						if (this->p_data->p_bitmap[no_full_bitmap] == 0)
						{
							// 每开辟一个新的模块使得end+1
							// 每回收一次end前一个模块就向前检查，调整end至新的已分配模块
							this->p_data->p_end = this->p_data->p_start + pos + 1;
						}
					}
					tmp_bit >>= 1;
				}
			}
			size_t earse(T* p)
			{
				size_t distance = (p - this->p_data->p_start) / sizeof(T);
				if (distance > this->capacity)
				{
					return -1;
				}
				// 这里记录除法的余数和结果
				size_t distance_remainder = distance % sizeof(T);
				size_t distance_result = distance % sizeof(T);
				// 将对应位置为0
				uint64_t tmp_bitmap = ~(1 << distance_remainder);
				this->p_data->p_bitmap[distance_result] &= tmp_bitmap;
			}
			inline void setCapacity(size_t capacity)// 设置容量
			{
				this->capacity = 0;
				if (sizeof(T) * capacity < __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_SIZE__)
					this->capacity = capacity;
			}
			inline bool isValid()// 检查是否有效
			{
				return  0 == this->capacity || nullptr == this->p_data;
			}
			bool isAllDealloced()// 检查是否为全回收
			{
				if (this->p_data == nullptr)
					return false;
				T tmp_full_bit = static_cast<T>(-1);
				for (size_t i = 0; i < this->bitmap_size; ++i)
				{
					if (0 != (this->p_data->p_bitmap[i] | tmp_full_bit))
					{
						return false;
					}
				}
				return true;
			}
			bool isNoFull()// 检查是否填充满了
			{
				T nops = static_cast<uint64_t>(-1);
				for (size_t i = 0; i < this->bitmap_size; ++i)
				{
					if ((this->p_data->p_bitmap[i] & nops) != nops)
					{
						return true;
					}
				}
				return false;
			}
			void swap(BitMapVector&& object)// 交换数据
			{
				BitMapVectorData* p_tmp = this->p_data;
				this->p_data = object.p_data;
				object.p_data = p_tmp;
			}
			bool isSubPointer(void* p)
			{
				T* p_tmp = static_cast<T*>(p);
				return p_tmp < this->p_data->p_end&& p_tmp >= this->p_data->p_start;
			}
		private:
			bool isNull()
			{
				if (nullptr == this->p_data)
				{
					this->bitmap_size = this->capacity / sizeof(T);
					this->p_data->p_bitmap = new T[this->capacity + this->bitmap_size];
					this->p_data = new BitMapVectorData;
					this->p_data->p_start = this->p_data->p_bitmap + this->bitmap_size;
					this->p_data->p_end = this->p_data->p_end;
					this->p_data->p_end_storage = this->p_data->p_start + this->capacity + 1;
					memset(this->p_data->p_start, 0, this->capacity);
					memset(this->p_data->p_bitmap, 0, this->bitmap_size);
					return true;
				}
				return false;
			}
			int find_no_full_bitmap(size_t reverse = 0)
			{
				size_t i = (this->bitmap_size - 1) & reverse;
				auto judged_fun = [&i, this]()->bool {
					return this->p_data->p_bitmap[i] != static_cast<T>(-1); };

				if (reverse)
				{
					for (; i >= 0; i -= 1)
					{
						if (judged_fun()) return i;
					}
				}
				else
				{
					for (; i < this->bitmap_size; i += 1)
					{
						if (judged_fun()) return i;
					}
				}

				return -1;
			}
		private:
			BitMapVectorData* p_data;
			size_t capacity;
			size_t bitmap_size;
		};
		class BitMapVectors
		{
		public:
			struct BitMapVectorsData
			{
				BitMapVectorsData** p_free_list; // 全回收的
				BitMapVectorsData** p_mem_list; // 已分配的
				BitMapVector* p_start;
				BitMapVector* p_end;
				BitMapVector* p_end_stoage;
			};
		public:
			BitMapVectors():p_data(this->p_data = new BitMapVectorsData)
			{
				this->p_data->p_start = new BitMapVector[__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
				this->p_data->p_end = this->p_data->p_start;
				this->p_data->p_end_stoage = this->p_data->p_start + __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ + 1;
				this->p_data->p_free_list = new BitMapVectorsData * [__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
				this->p_data->p_mem_list = new BitMapVectorsData * [__MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__];
				memset(this->p_data->p_free_list, reinterpret_cast<int>(nullptr), __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__);
				memset(this->p_data->p_mem_list, reinterpret_cast<int>(nullptr), __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__);
			}
			BitMapVectors(const BitMapVectors&) = delete;
			BitMapVectors(BitMapVectors&& object) noexcept
			{
				BitMapVectorsData* p_tmp = this->p_data;
				this->p_data = object.p_data;
				object.p_data = p_tmp;
			}
			~BitMapVectors()
			{
				if (this->p_data != nullptr)
				{
					delete[] this->p_data->p_start;
					delete[] this->p_data->p_mem_list;
					delete[] this->p_data->p_free_list;
					memset(this->p_data, reinterpret_cast<int>(nullptr), 5);
					delete this->p_data;
				}
			}
		public:
			int push_back(size_t array_size)// 负责创建对应的BitmapVector
			{
				size_t pos = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
				if (pos > __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ || pos < 0)
				{
					// 越界情况
				}
				this->p_data->p_start[pos].setCapacity(array_size);
			}
			void pop_back();// 负责标记最后一个为0
			bool compare_by_array_size();// 用于排满后进行和全回收内容的长度对比
			void swap();
		private:
			bool isNull();
		private:
			void* allocate()
			{
				size_t i = 0;
				for (; i < __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__; ++i)
				{
					if (this->p_data->p_start[i].isNoFull())// 寻找不是全分配的 不是就直接在这分配一块出去
					{
						return static_cast<T*>(this->p_data->p_start[i][this->p_data->p_start[i].push_back()]);
					}
					// 如果都是全分配的那就新创建一个
					// this->push_back();
				}
				return nullptr;
			}
			void deallocate(void* p)
			{
				size_t bitmap_vector_count = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
				for (size_t i = 0; i < bitmap_vector_count; ++i)
				{
					if (this->p_data->p_start[i].isSubPointer(p))
					{
						this->p_data->p_start[i].earse(static_cast<uint64_t*>(p));
					}
				}
			}
		private:
			BitMapVectorsData* p_data;
			size_t allocated_num;
		};
	private:
		BitMapVectors bitmap_data;
		void* bitmap_allocate(size_t size);
		void bitmap_deallocate(void* p);
	public:
		void* allocate(size_t size) override
		{
			return bitmap_allocate(size);
		}
		void deallocate(void* p, size_t size = 0) override
		{
			bitmap_deallocate(p);
		}
	};
#endif //__MUZI_ALLOCATOR_MOD_BITMAP__


	class MAllocator1
	{
	public:// 公用数据
		static size_t object_num;
		// 规定每种模式都需要传递一个初始化函数到此处
		using MemoryCtrlFunction = void(*)(void*);
		static MemoryCtrlFunction mcf[__MUZI_ALLOCATOR_MOD_SIZE__];
		static void* mcf_arg[__MUZI_ALLOCATOR_MOD_SIZE__];
		// 规定每种模式都需要传递一个清理函数到此处
		using clearMemoryFunction = void(*)(void*);
		static clearMemoryFunction cmf[__MUZI_ALLOCATOR_MOD_SIZE__];
		static void* cmf_arg[__MUZI_ALLOCATOR_MOD_SIZE__];

	public:// 公用构造函数
	};

};




#endif // __MUZI_MALLOCATOR_H__