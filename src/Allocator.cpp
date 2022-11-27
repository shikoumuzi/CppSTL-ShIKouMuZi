#include"Allocator.h"

namespace MUZI {
#ifdef __MUZI_ALLOCATOR_MOD_POOL__
	size_t MAllocator::object_num = 0;
	size_t MAllocator::mem_total = 0;
	union MAllocator::MAllocatorRep* MAllocator::mem_pool[__MUZI_ALLOCAOTR_MOD_POOL_SPECIFICATION_COUNT__] = { nullptr };
	union MAllocator::MAllocatorRep* MAllocator::start_free_pool_ptr = nullptr;
	union MAllocator::MAllocatorRep* MAllocator::end_free_pool_ptr = nullptr;
#endif // __MUZI_ALLOCATOR_MOD_POOL__


};
