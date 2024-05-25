#pragma once
#ifndef __MUZI_MATOMICLOCK_H__
#define __MUZI_MATOMICLOCK_H__
#include<atomic>

namespace MUZI
{
	class MAtomicLock
	{
	public:
		MAtomicLock() {}
		~MAtomicLock() {};
	public:
		inline void lock()
		{
			while(this->m_lock.test_and_set());
		}
		inline void unlock()
		{
			this->m_lock.clear();
		}
		inline bool try_lock()
		{
			return this->m_lock.test();
		}
	private:
		std::atomic_flag m_lock;
	};
}

#endif // !__MUZI_MATOMICLOCK_H__
