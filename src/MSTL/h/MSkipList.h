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
		~MSkipList()
		{}
	public:
		/// @brief this function will copy all nodes(data) from other list
		/// @param list other object which type is MSkipList&
		void operator+=(const MSkipList<T>& list)
		{}
		/// @brief this function will connect the end of node from 'this' with the front of node from list, and the list will be empty
		/// @param list other object which type is MSkipList&&
		void operator+=(MSkipList<T>&& list)
		{}
		/// @brief thie function will copy all data from other list
		/// @param list other object which type is std::span<T>&&
		void operator+=(std::span<T>& list)
		{}
		void operator+(const MSkipList<T>& list)
		{}
		/// @brief this function will connect the end of node from 'this' with the front of node from list, and the list will be empty
		/// @param list other object which type is MSkipList&&
		void operator+(MSkipList<T>&& list)
		{}
		/// @brief thie function will copy all data from other list
		/// @param list other object which type is std::span<T>&&
		void operator+(std::span<T>& list)
		{}
		void operator=(const MSkipList<T>&)
		{}
		void operator=(MSkipList<T>&&)
		{}
	public: // 增删查改
		void push_back()
		{}
		void pop_back()
		{}
		void push_front()
		{}
		void pop_front()
		{}
		void emplace_back()
		{}
		void emplace_front()
		{}
		void insert()
		{}
		void emplace()
		{}
		void find()
		{}
		void erase()
		{}
		void find_if()
		{}
		void erase_if()
		{}
	public: // 获取信息 和 迭代器
		void data()
		{}
		void size()
		{}
		void length()
		{}
		void empty()
		{}
		void begin()
		{}
		void end()
		{}
	public: // 简易内存操作
		void resize()
		{}
		void reserve()
		{}
		void clear()
		{}

	private:
		MAtomicLock m_atomic_lock;
		__MSkipList_Node__<T>* m_header;
		MBitmapAllocator<__MSkipList_Node__<T>> m_allocator;
	};
}

#endif // !__MUZI_MSKIPLIST_H__
