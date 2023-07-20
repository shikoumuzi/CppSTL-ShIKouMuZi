#pragma once
#ifndef __MUZI_MSYNCANNULARQUEUE_H__
#define __MUZI_MSYNCANNULARQUEUE_H__

#define __MUZI_MSYNCANNULARQUEUE_DEFAULT_CAPACITY__ 1024
#include<atomic>
namespace MUZI
{
	template<typename T, size_t CAPACITY = __MUZI_MSYNCANNULARQUEUE_DEFAULT_CAPACITY__>
	class MSyncAnnularQueue// 环形缓冲队列
	{
	private:
		enum OPERATOR_TYPE
		{
			PUSH,
			POP
		};
	public:
		template<typename T>
		struct MSyncAnnularQueueNode
		{
			T ele;
			struct MSyncAnnularQueueNode<T>* last;
			struct MSyncAnnularQueueNode<T>* next;
		};
	public:
		MSyncAnnularQueue()
			:m_data(new MSyncAnnularQueueNode<T>[CAPACITY])
		{
			for (int i = 1; i < CAPACITY - 1; ++i)
			{
				this->m_data[i].next = &this->m_data[i + 1];
				this->m_data[i].last = &this->m_data[i - 1];
			}
			this->m_data[CAPACITY - 1].next = &this->m_data[0];
			this->m_data[CAPACITY - 1].last = &this->m_data[CAPACITY - 2];
			this->m_data[0].next = &this->m_data[1];
			this->m_data[0].last = &this->m_data[CAPACITY - 1];
			this->m_begin = this->m_data;
			this->m_end = this->m_data;

		}
		~MSyncAnnularQueue()
		{
			delete this->m_data;
			this->m_data = nullptr;
			this->m_begin = nullptr;
			this->m_end = nullptr;
		}

	public:
		void push(const T& ele)
		{
			while (this->m_lock.test_and_set());

			// 采用双向循环链表，天然的循环队列
			if (this->m_end->next == this->m_begin)
			{
				this->m_begin = this->m_begin->next;
			}
			this->m_end = this->m_end->next;
			this->m_end->ele = ele;

			this->m_lock.clear();
		}
		void pop()
		{
			while (this->m_lock.test_and_set());
			
			if (this->m_begin->last == this->m_end)
			{
				this->m_end = this->m_end->last;
			}
			this->m_begin = this->m_begin->last;

			this->m_lock.clear();
		}
	public:
		T& front()
		{
			return this->m_begin->ele;
		}
	public:
		bool empty()
		{
			return this->m_begin == this->m_end;
		}
	private:
		std::atomic_flag m_lock;
		struct MSyncAnnularQueueNode<T>* m_data;
		struct MSyncAnnularQueueNode<T>* m_begin;
		struct MSyncAnnularQueueNode<T>* m_end;
	};

}








#endif // !__MUZI_MSYNCQUEUE_H__


