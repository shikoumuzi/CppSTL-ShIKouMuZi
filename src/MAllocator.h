#pragma once

#ifndef __MUZI_MALLOCATOR_H__
#define __MUZI_MALLOCATOR_H__

#include<stdlib.h>
#include<memory>
#include<new>
#include<atomic>
//����ģʽѡ�񣬼�ѡ����Ҫ���ݽṹ
#define __MUZI_ALLOCATOR_MOD_SIZE__ 4
#define __MUZI_ALLOCATOR_MOD_POOL__ 0
#define __MUZI_ALLOCATOR_MOD_BITMAP__ 1
#define __MUZI_ALLOCATOR_MOD_LOKI__ 2
#define __MUZI_ALLOCATOR_MOD_ARRAY__ 3

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

#ifdef __MUZI_ALLOCATOR_MOD_ARRAY__
#define __MUZI_ALLOCATOR_MOD_ARRAY_REDEALLOCATED_TIMES__ 5
#endif // __MUZI_ALLOCATOR_MOD_ARRAY__

#ifdef __MUZI_ALLOCATOR_MOD_BITMAP__
#define __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_SIZE__ 4194304U /*һ�������ȥ��������󳤶�Ϊ4k ��һҳ*/
#define __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_COUNT__ 32768U /*һ����������Ԫ�ظ���*/
#define __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ 64
#define __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__ uint64_t
#define __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE_SIZE__ sizeof(__MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE__)
#define __MUZI_ALLOCATOR_MOD_BITMAP_BLOCK_TYPE_BIT_SIZE__ sizeof(T) * 8
#define __MUZI_ALLOCATOR_MOD_BITMAP_FIRST_ALLOCATED_ARRAY_SIZE__ 128
#define __MUZI_ALLOCATOR_MOD_BITMAP_INCREASEING_MULTIPLE__ 2
#endif // __MUZI_ALLOCATOR_MOD_BITMAP__




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
#ifdef __MUZI_ALLOCATOR_MOD_POOL__ // ��ͬģʽ�µĲ�ͬ���ݽṹ�Ͷ�Ӧ�㷨

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
			// ArraryAllocateArgs��
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
	// �������ͨPOOL ������������������ķ�ʽ�����ڴ��
	// �ŵ��ǿ�����ʱ���ڴ���Դ�黹������ϵͳ���򵥾������ӻ��黹����
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
			BitMapVector(size_t capacity) :p_data(nullptr)//��������ж��ٿ�
			{
				// ��ǰ����֤capacityΪsizeof(T)�ı���
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
			int push_back()// ������һ�����õ�
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
							// ÿ����һ���µ�ģ��ʹ��end+1
							// ÿ����һ��endǰһ��ģ�����ǰ��飬����end���µ��ѷ���ģ��
							this->p_data->p_end = this->p_data->p_start + pos + 1;
						}
						this->p_data->p_bitmap[no_full_bitmap] |= tmp_bit;

					}
					tmp_bit >>= 1;
				}
				return pos;
			}
			void pop_back()// ������һ��Ϊ�ѻ����ڴ��
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
							// ÿ����һ���µ�ģ��ʹ��end+1
							// ÿ����һ��endǰһ��ģ�����ǰ��飬����end���µ��ѷ���ģ��
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
				// �����¼�����������ͽ��
				size_t distance_remainder = distance % sizeof(T);
				size_t distance_result = distance % sizeof(T);
				// ����Ӧλ��Ϊ0
				uint64_t tmp_bitmap = ~(1 << distance_remainder);
				this->p_data->p_bitmap[distance_result] &= tmp_bitmap;
			}
			inline void setCapacity(size_t capacity)// ��������
			{
				this->capacity = 0;
				if (sizeof(T) * capacity < __MUZI_ALLOCATOR_MOD_BITMAP_VECTOR_MAX_SIZE__)
					this->capacity = capacity;
			}
			inline bool isValid()// ����Ƿ���Ч
			{
				return  0 == this->capacity || nullptr == this->p_data;
			}
			bool isAllDealloced()// ����Ƿ�Ϊȫ����
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
			bool isNoFull()// ����Ƿ��������
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
			void swap(BitMapVector&& object)// ��������
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
				BitMapVectorsData** p_free_list; // ȫ���յ�
				BitMapVectorsData** p_mem_list; // �ѷ����
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
			int push_back(size_t array_size)// ���𴴽���Ӧ��BitmapVector
			{
				size_t pos = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
				if (pos > __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ || pos < 0)
				{
					// Խ�����
				}
				this->p_data->p_start[pos].setCapacity(array_size);
			}
			void pop_back();// ���������һ��Ϊ0
			bool compare_by_array_size();// ������������к�ȫ�������ݵĳ��ȶԱ�
			void swap();
		private:
			bool isNull();
		private:
			void* allocate()
			{
				size_t i = 0;
				for (; i < __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__; ++i)
				{
					if (this->p_data->p_start[i].isNoFull())// Ѱ�Ҳ���ȫ����� ���Ǿ�ֱ���������һ���ȥ
					{
						return static_cast<T*>(this->p_data->p_start[i][this->p_data->p_start[i].push_back()]);
					}
					// �������ȫ������Ǿ��´���һ��
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
	};

};




#endif // __MUZI_MALLOCATOR_H__