#include"MAllocator.h"
#ifndef __MUZI_MBITMAPALLOCATOR_H__
#define __MUZI_MBITMAPALLOCATOR_H__

namespace MUZI {
	// 相比起普通POOL 这里采用数组代替链表的方式保管内存块
		// 优点是可以随时将内存资源归还给操作系统、简单精简、有延缓归还能力
		//
	//template<typename T>
	//concept __muzi_alloctor_mod_bitmap_data_type__ = requires(T x)
	//{
	//	sizeof(T) <= sizeof(uint64_t) * 2;
	//};

	template<typename T = uint64_t>
	class MBitmapAllocator :public MAllocator
	{
	public:
		template<typename T>
		static MAllocator* getMAllocator(void* p = nullptr)
		{
			return dynamic_cast<MAllocator*>(new MBitmapAllocator<T>());
		}

	public:
		MBitmapAllocator()
		{}
		MBitmapAllocator(MBitmapAllocator&& allocator) {}
		~MBitmapAllocator()
		{}
	private:
		//template<typename T>
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
				return p_tmp < this->p_data->p_end && p_tmp >= this->p_data->p_start;
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

		//template<typename T>
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
			BitMapVectors() :p_data(this->p_data = new BitMapVectorsData)
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
			void pop_back() {}// 负责标记最后一个为0
			bool compare_by_array_size() {}// 用于排满后进行和全回收内容的长度对比
			void swap() {}
		private:
			bool isNull() {}
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

		//template<typename T>
		class MBitmapMemory
		{
		public:
			MBitmapMemory()
			{}
		public:
			T* next()
			{}
		public:
			size_t size;
		};
	private:
		BitMapVectors bitmap_data;
		void* bitmap_allocate(size_t size)
		{/*未实现*/ return nullptr;
		}
		void bitmap_deallocate(void* p)
		{/*未实现*/
		}
	public:
		/// @brief this function will alloacte memory
		/// @param size block size, it will allocate a block of memory which capacity is sizeof(T) * size
		/// @return the pointer which pointer a block of memory
		void* allocate(size_t size) override
		{
			return bitmap_allocate(size);
		}
		void deallocate(void* p, size_t size = 0) override
		{
			bitmap_deallocate(p);
		}
		void clear() {}
		void extend(MBitmapAllocator&&) {}
		void receive(MBitmapAllocator&&) {}
	};
}
#endif // !__MUZI_MBITMAPALLOCATOR_H__