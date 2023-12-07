#include"MAllocator.h"
#ifndef __MUZI_MBITMAPALLOCATOR_H__
#define __MUZI_MBITMAPALLOCATOR_H__

namespace MUZI {
	// �������ͨPOOL ������������������ķ�ʽ�����ڴ��
		// �ŵ��ǿ�����ʱ���ڴ���Դ�黹������ϵͳ���򵥾������ӻ��黹����
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
				BitMapVectorsData** p_free_list; // ȫ���յ�
				BitMapVectorsData** p_mem_list; // �ѷ����
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
			int push_back(size_t array_size)// ���𴴽���Ӧ��BitmapVector
			{
				size_t pos = (this->p_data->p_end - this->p_data->p_start) / sizeof(BitMapVector);
				if (pos > __MUZI_ALLOCATOR_MOD_BITMAP_BITMAPVECTORS_SIZE__ || pos < 0)
				{
					// Խ�����
				}
				this->p_data->p_start[pos].setCapacity(array_size);
			}
			void pop_back() {}// ���������һ��Ϊ0
			bool compare_by_array_size() {}// ������������к�ȫ�������ݵĳ��ȶԱ�
			void swap() {}
		private:
			bool isNull() {}
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
		{/*δʵ��*/ return nullptr;
		}
		void bitmap_deallocate(void* p)
		{/*δʵ��*/
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