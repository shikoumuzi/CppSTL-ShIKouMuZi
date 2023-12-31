#pragma once
#ifndef __MUZI_MPRIORITYQUEUE_H__
#define __MUZI_MPRIORITYQUEUE_H__
#include"MSkipList.h"
#include"MBase/MObjectBase.h"
#include<compare>
#include<concepts>
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
			return (this->priority == node.priority) && this->value == this->value;
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

	template<template <typename V> typename Node = __MPriorityQueueNode__>
	class __MPriorityQueueNode_Wrapper__
	{
		template<typename V>
		using node_type = Node<V>;
	};

	template<template<typename...> class Container = MSkipList, typename Elem = __MDefaultTypeDefine__>
	concept __MPriorityQueueContainer__ = requires(Container<__MPriorityQueueNode__<Elem>> x, __MPriorityQueueNode__<Elem> node, Elem ele)
	{
		typename Container<__MPriorityQueueNode__<Elem>>::value_type;
		typename Container<__MPriorityQueueNode__<Elem>>::size_type;
		typename Container<__MPriorityQueueNode__<Elem>>::iterator;
		{x.insert(node)} -> std::same_as<void>;
		{x.size()} -> std::same_as<size_t>;
		{x.front()} -> std::same_as<Elem&>;
		{x.erase(node)} -> std::same_as<void>;
		{x.begin()} -> std::same_as<typename Container<__MPriorityQueueNode__<Elem>>::iterator>;
		{x.end()} -> std::same_as<typename Container<__MPriorityQueueNode__<Elem>>::iterator>;
		{x.clear()} -> std::same_as<void>;
		std::is_same_v<typename Container<__MPriorityQueueNode__<Elem>>::value_type, __MPriorityQueueNode__<Elem>>;
	};

	template<typename T = __MDefaultTypeDefine__, template<typename> typename Container = MSkipList>
		requires __MPriorityQueueContainer__<Container, T>// 无法直接限定，需要通过后接requires来进行限定
	class MPriorityQueue
	{
	public:
		using value_type = T;
		using node_type = struct __MPriorityQueueNode__<T>;
		using container_type = Container<node_type>;
	public:
		MPriorityQueue()
		{}
		~MPriorityQueue()
		{}
	public:
		void push(const T& ele, int priority = 0)
		{
			this->m_container.insert(node_type(ele, priority));
		}
		void pop()
		{
			this->m_container.erase(this->m_container.begin());
		}
		void clear()
		{
			this->m_container.clear();
		}
	public:
		size_t size()
		{
			return this->m_container.size();
		}
		T& front()
		{
			return this->m_container.front();
		}

	public:
		size_t size() const
		{
			return this->m_container.size();
		}
		const T& front() const
		{
			return this->m_container.front();
		}

	private:
		container_type m_container;
	};
}

#endif // !__MUZI_PRIORITYQUEUE_H__
