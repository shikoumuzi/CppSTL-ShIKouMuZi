#pragma once
#ifndef __MUZI_MSKIPLIST_H__
#define __MUZI_MSKIPLIST_H__
#include<initializer_list>
#include"MAtomicLock.h"
#include<span>
#include<random>
#include"MAllocator/MBitmapAllocator.h"
namespace MUZI
{
	template<typename T>
	class MSkipList
	{
	private:
		template<T>
		struct __MSkipList_Node__
		{
		private:
			static size_t randLevel(int max_level)
			{
				return static_cast<size_t>(std::rand() % max_level + 1);
			}
		public:
			__MSkipList_Node__() :
				value(),
				next(nullptr),
				index_level(0)
			{}
			__MSkipList_Node__(__MSkipList_Node__<T>* next, int max_level) :
				value(),
				next(next),
				index_level(this->randLevel(max_level))
			{}
			__MSkipList_Node__(const T& value, __MSkipList_Node__<T>* next, int max_level) :
				value(value),
				next(next),
				index_level(this->randLevel(max_level))
			{}
			__MSkipList_Node__(const __MSkipList_Node__& node) :
				value(node.value),
				next(node.next),
				index_level(node.index_level)
			{}
		public:
			T value;
			__MSkipList_Node__<T>* next;
			size_t index_level;
		};
	public:
		MSkipList()
		{}
		MSkipList(const MSkipList<T>&)
		{}
		MSkipList(MSkipList<T>&&)
		{}
		MSkipList(int size)
		{}
		MSkipList(const std::initializer_list<T>&)
		{}
	public:
		void operator+=(const MSkipList&)
		{}
		void operator+=(MSkipList&& list)
		{}
	private:
		MAtomicLock m_atomic_lock;
		__MSkipList_Node__<T>* m_header;
		MBitmapAllocator<__MSkipList_Node__<T>> m_allocator;
	};
}

#endif // !__MUZI_MSKIPLIST_H__
