#ifndef __MUZI_MARRAYALLOCATOR_H__
#define __MUZI_MARRAYALLOCATOR_H__
#include"MAllocator.h"
namespace MUZI {
	class MArrayAllocator : public MAllocator
	{
	public:
		class ArraryAllocateArgs
		{
		public:
			friend class MArrayAllocator;
		public:
			// ArraryAllocateArgs¼þ
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
		void* array_allocate(size_t block_num)
		{
			return nullptr;
		}
		void array_deallocate(void* p)
		{

		}
		void dellocate_Rep()
		{

		}
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
}
#endif // !__MUZI_MARRAYALLOCATOR_H__
