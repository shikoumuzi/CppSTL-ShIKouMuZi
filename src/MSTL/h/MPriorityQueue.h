#pragma once
#ifndef __MUZI_MPRIORITYQUEUE_H__
#define __MUZI_MPRIORITYQUEUE_H__
#include"MSkipList.h"
#include"MBase/MObjectBase.h"
#include<compare>
namespace MUZI
{
	template<typename T = __MDefaultTypeDefine__>
	struct __MPriorityQueueNode__
	{
	public:
		friend class MPriorityQueue<T>;
	public:
		__MPriorityQueueNode__(int32_t priority, T value) :
			priority(priority),
			value(value)
		{}
	public:
		std::weak_ordering operator<=>(const __MPriorityQueueNode__& node)
		{
			// 为了匹配MSkipList，其内部是实行按升序排序的，采用相反的进行降序排序
			return !this->priority <=> node.priority;
		}
		bool operator==(const __MPriorityQueueNode__& node)
		{
			return this->priority == node.priority;
		}
	public:
		std::weak_ordering operator<=>(const __MPriorityQueueNode__& node) const
		{
			return !this->priority <=> node.priority;
		}
		bool operator==(const __MPriorityQueueNode__& node) const
		{
			return this->priority == node.priority;
		}
	private:
		int32_t priority;
		T value;
	};

	template<typename T = __MDefaultTypeDefine__>
	class MPriorityQueue : private MSkipList<struct __MPriorityQueueNode__<T>>
	{
	public:
		using value_type = T;
		using node_type = struct __MPriorityQueueNode__<T>;
	public:
		MPriorityQueue()
		{}
		~MPriorityQueue()
		{}
	public:
		void push(const T& ele, int priority)
		{
			this->MSkipList<node_type>::insert(node_type(priority, ele));
		}
		void pop()
		{
			this->MSkipList<node_type>::erase(this->MSkipList<node_type>::front());
		}

	public:
		size_t size()
		{
			return this->MSkipList<node_type>::size();
		}
	public:
		size_t size() const
		{
			return this->MSkipList<node_type>::size();
		}
	};
}

#endif // !__MUZI_PRIORITYQUEUE_H__
