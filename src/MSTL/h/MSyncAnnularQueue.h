#pragma once
#ifndef __MUZI_MSYNCANNULARQUEUE_H__
#define __MUZI_MSYNCANNULARQUEUE_H__

#define __MUZI_MSYNCANNULARQUEUE_DEFAULT_CAPACITY__ 1024
#include<atomic>
#include<concepts>
#include"MAtomicLock.h"
namespace MUZI
{
	template<typename T>
	concept __MSYNCANNULARQUEUE_ELE_TYPE__ = requires(T x, T y)
	{
		{x = y};

	};

	template<typename T>
	concept __MSYNCANNULARQUEUE_LOCK_TYPE__ = requires(T x)
	{
		{x.lock()}->std::same_as<void>;
		{x.unlock()}->std::same_as<void>;
		{x.try_lock()}->std::same_as<bool>;

	};

	template<
		__MSYNCANNULARQUEUE_ELE_TYPE__ T, 
		size_t CAPACITY = __MUZI_MSYNCANNULARQUEUE_DEFAULT_CAPACITY__, 
		__MSYNCANNULARQUEUE_LOCK_TYPE__ LOCK_TYPE = MAtomicLock>
	class MSyncAnnularQueue// 环形缓冲队列
	{
	public:
		using lock_type = LOCK_TYPE;
		using value_type = T;
		using reference = T&;
		using const_reference = const T&;
	public:
		template<typename T>
		struct MSyncAnnularQueueNode
		{
			T ele;
			struct MSyncAnnularQueueNode<T>* next;
		};
	public:
		MSyncAnnularQueue()
			:m_data(new MSyncAnnularQueueNode<T>[CAPACITY])
		{
			// 初始化循环链表
			for (int i = 0; i < CAPACITY - 1; ++i)
			{
				this->m_data[i].next = &this->m_data[i + 1];
			}
			this->m_data[CAPACITY - 1].next = &this->m_data[0];
			this->m_begin = this->m_data;
			this->m_end = this->m_data;

		}
		~MSyncAnnularQueue()
		{
			delete[] this->m_data;
			this->m_data = nullptr;
			this->m_begin = nullptr;
			this->m_end = nullptr;
		}

	public:
		void push(const T& ele)
		{
			this->m_lock.lock();

			// 采用单向循环链表，如果满队列再push会直接覆盖数据
			if (this->m_end->next == this->m_begin)
			{
				this->m_begin = this->m_begin->next;
			}
			this->m_end->ele = ele;
			this->m_end = this->m_end->next;

			this->m_lock.unlock();
		}
		void pop()
		{
			this->m_lock.lock();
			// 代表该队列目前仍然没有值
			if (this->m_begin == this->m_end)
			{
				this->m_lock.unlock();
				return;
			}
			this->m_begin = this->m_begin->next;

			this->m_lock.unlock();
		}
	public:
		T* front()
		{
			T* ret_ptr = nullptr;
			this->m_lock.lock();
			if (this->m_begin == this->m_end)
			{
				this->m_lock.unlock();
				return nullptr;
			}
			ret_ptr = &this->m_begin->ele;

			this->m_lock.unlock();

			return ret_ptr;
		}
		//T* back()
		//{
		//	T* ret_ptr = nullptr;
		//	this->m_lock.lock();
		//	if (this->m_begin == this->m_end)
		//	{
		//		this->m_lock.unlock();
		//		return nullptr;
		//	}
		//	// 如果循环队列尾端在数据头部，则取数据末尾元素
		//	if (this->m_end == this->m_data)
		//	{
		//		ret_ptr = &((this->m_data + CAPACITY)->ele);
		//		this->m_lock.unlock();

		//		return ret_ptr;
		//	}
		//	
		//	// 其他情况则是end前一位
		//	ret_ptr = &((this->m_end - 1)->ele);

		//	this->m_lock.unlock();

		//	return ret_ptr;
		//}
	public:
		bool empty()
		{
			return this->m_begin == this->m_end;
		}
	private:
		lock_type m_lock;
		struct MSyncAnnularQueueNode<T>* m_data;
		struct MSyncAnnularQueueNode<T>* m_begin;
		struct MSyncAnnularQueueNode<T>* m_end;// 代表的是一个单独的末尾元素，其内部不包含可用值
	};

}








#endif // !__MUZI_MSYNCQUEUE_H__


