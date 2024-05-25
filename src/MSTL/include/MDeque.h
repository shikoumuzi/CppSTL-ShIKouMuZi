#ifndef __MUZI_MQUEUE_H__
#define __MUZI_MQUEUE_H__
#include"../MAllocator/MPoolAllocator.h"
namespace MUZI
{
	template<typename T>
	class MDeque
	{
	public:
		
	public:
		MDeque():alloc(MPoolAllocator::getMAllocator())
		{

		}
		~MDeque()
		{

		}

	private:
		MPoolAllocator* alloc;

	};


}


#endif // !__MUZI_MQUEUE_H__
