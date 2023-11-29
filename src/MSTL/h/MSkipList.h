#pragma once
#ifndef __MUZI_MSKIPLIST_H__
#define __MUZI_MSKIPLIST_H__
#include<initializer_list>
#include"MAtomicLock.h"
#include<span>
#include<random>
#include"MAllocator/MBitmapAllocator.h"
#include<iterator>
#include<atomic>
#define __MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__ 3 /*��ʼ����������*/
#define __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__ 1.5 /*�ڴ�������չϵ��*/
#define __MUZI_MSKIPLIST_DEFAULT_INDEX_LEVEL_UP_COEFFICIENT__ 2000 /*�����㼶�����߽�ֵ*/

namespace MUZI
{
	template<typename T = int>
	class MSkipList
	{
	public:
		using value_type = T;
	private:
		template<typename T>
		struct __MSkipListIndexBround__
		{
			struct MSkipList<T>::__MSkipListNode__* pointer;
		};
		template<typename T>
		struct __MSkipListNode__
		{
		private:
			static size_t randLevel(int max_level)
			{
				return static_cast<size_t>(std::rand() % max_level + 1);
			}
		public:
			__MSkipListNode__() :
				value(),
				next(nullptr),
				index_level(0),
				index_next(nullptr)
			{}
			__MSkipListNode__(__MSkipListNode__<T>* next, int max_level) :
				value(),
				next(next),
				index_level(this->randLevel(max_level)),
				index_next(new __MSkipListNode__<T>* [index_level] {nullptr})
			{}
			__MSkipListNode__(const T& value, __MSkipListNode__<T>* next, int max_level) :
				value(value),
				next(next),
				index_level(this->randLevel(max_level)),
				index_next(new __MSkipListNode__<T>*[index_level] {nullptr})
			{}
			__MSkipListNode__(T&& value, __MSkipListNode__<T>* next, int max_level) :
				value(value),
				next(next),
				index_level(this->randLevel(max_level)),
				index_next(new __MSkipListNode__<T>* [index_level] {nullptr})
			{}
			__MSkipListNode__(const __MSkipListNode__& node) :
				value(node.value),
				next(node.next),
				index_level(node.index_level),
				index_next(new __MSkipListNode__<T>*[index_level] {nullptr})
			{}
			~__MSkipListNode__()
			{
				delete[] this->index_next;
				this->index_next = nullptr;
				this->next = nullptr;
			}
		public:
			T value; // ֵ
			__MSkipListNode__<T>* next; // ��һ���ڵ�
			__MSkipListNode__<T>** index_next; // ������һ���ڵ�
			size_t index_level; // ��������
			std::atomic<bool> m_writing_flag; // �����ڲ����������жϵ�ǰ����Ƿ���Ҫ�޸�
		};
	public:
		MSkipList() :
			m_size(0),
			m_capacity(0),
			m_header(nullptr),
			m_tail(nullptr),
			m_null_node_header(nullptr),
			m_max_level(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_level_tail(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_level_header(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
		{
			for (size_t i = 0; i < __MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__; ++i)
			{
				this->m_level_header[i].pointer = nullptr;
				this->m_level_tail[i].pointer = nullptr;
			}
		}
		MSkipList(const MSkipList<T>& list) :
			m_allocator(),
			m_size(list.m_size),
			m_capacity(list.m_size),
			m_max_level(list.m_max_level),
			m_level_tail(list.m_max_level),
			m_level_header(list.m_max_level),
			m_null_node_header(nullptr),
			m_header(this->m_allocator.allocate(list.m_size))
		{
			// �з�����ȡ�����ڴ�ռ�
			size_t i = 0;
			for (__MSkipListNode__<T>* tmp_header = list.m_header; tmp_header->next != nullptr && tmp_header != nullptr; tmp_header = tmp_header->next)
			{
				(this->m_header + i)->next = &(this->m_header + i + 1);
				new(&(this->m_header->value)) T(tmp_header->value);
				(this->m_header + i)->index_level = tmp_header->index_level;
				if (tmp_header == list.m_tail)
				{
					this->m_tail == this->m_header + i;
				}
				i += 1;
			}
			(this->m_header + i)->next = nullptr;

			// ����������һ��
			this->m_level_header[0].pointer = this->m_header;
			this->m_level_tail[0].pointer = (this->m_header + i);
			// ����ʣ����������
			// ��ѭ���������в㼶
			for (size_t i = 1; i < list.m_max_level; ++i)
			{
				__MSkipListNode__<T>* tmp_index_ptr = this->m_level_header[i].pointer;
				__MSkipListNode__<T>* tmp_ptr = this->m_header;
				// ��ѭ��ɨ�����нڵ�
				for (; tmp_ptr != nullptr && tmp_ptr->next != nullptr; tmp_ptr = tmp_ptr->next)
				{
					if (tmp_ptr->index_level != 1 && tmp_ptr->index_level < i + 1)
					{
						tmp_index_ptr->next = tmp_ptr;
						tmp_index_ptr = tmp_ptr;
					}
				}
				this->m_level_tail[i].pointer = tmp_ptr;
			}

		}
		MSkipList(MSkipList<T>&& list):
			m_allocator(std::move(list.m_allocator)),
			m_size(list.m_size),
			m_capacity(list.m_size),
			m_max_level(list.m_max_level),
			m_level_tail(list.m_level_tail),
			m_level_header(list.m_level_header),
			m_null_node_header(list.m_null_node_header),
			m_header(list.m_header),
			m_tail(list.m_tail)
		{
			list.m_size = 0;
			list.m_capacity = 0;
			list.m_header = nullptr;
			list.m_null_node_header = nullptr;
			list.m_max_level = __MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__;
			list.m_level_header.clear();
			list.m_level_tail.clear();
			list.m_tail = nullptr;
		}
		MSkipList(int size):
			m_allocator(),
			m_size(size),
			m_capacity(size),
			m_level_header(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_level_tail(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_max_level(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_null_node_header(nullptr),
			m_header(this->m_allocator.allocate(size))
		{
			for (size_t i = 0; i < size - 1; ++i)
			{
				(this->m_header + i)->next = &(this->m_header + i + 1);
				new(&(this->m_header->value)) T();
				(this->m_header + i)->index_level = __MSkipListNode__<T>::randLevel(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__);
			}
			(this->m_header + i)->next = nullptr;
		}
		MSkipList(const std::initializer_list<T>& list) :
			m_allocator(),
			m_size(list.size()),
			m_capacity(list.size()),
			m_max_level(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_level_header(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_level_tail(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_null_node_header(nullptr),
			m_header(this->m_allocator.allocate(list.size()))
		{
			size_t i = 0;
			for (auto it = list.begin(); it != list.end() - 1; ++it)
			{
				(this->m_header + i)->next = &(this->m_header + i + 1);
				new(&(this->m_header->value)) T(x);
				(this->m_header + i)->index_level = __MSkipListNode__<T>::randLevel(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__);
				i += 1;
			}
			(this->m_header + i)->next = nullptr;
		}
		~MSkipList()
		{}
	public:
		/// @brief this function will copy all nodes(data) from other list
		/// @param list other object which type is MSkipList&
		void operator+=(const MSkipList<T>& list)
		{
			
		}
		/// @brief this function will connect the end of node from 'this' with the front of node from list, and the list will be empty
		/// @param list other object which type is MSkipList&&
		void operator+=(MSkipList<T>&& list)
		{

			this->m_tail->next = list.m_header;
			this->m_tail = list.m_tail;
			this->m_size += list.m_size;
			this->m_capacity += list.m_size;
			if (this->m_max_level > list.m_max_level)
			{

			}
			
		}
		/// @brief this function will copy all nodes(data) from other list
		/// @param list other object which type is const std::span&
		void operator+=(const std::span<T>& list)
		{

		}
		/// @brief this function will copy all nodes(data) from other list and return a new object
		/// @param list other object which type is MSkipList&
		MSkipList<T> operator+(const MSkipList<T>& list)
		{
			MSkipList<T> ret_list(*this);
			ret_list += list;
			return ret_list;
		}
		/// @brief this function will connect the end of node from 'this' with the front of node from list, and the list will be empty and return a new object 
		/// @param list other object which type is MSkipList&&
		void operator+(MSkipList<T>&& list)
		{
			MSkipList<T> ret_list(*this);
			ret_list += std::move(list);
			return ret_list;
		}
		/// @brief this function will copy all nodes(data) from other list and return a new object
		/// @param list other object which type is std::span
		void operator+(std::span<T>& list)
		{
			MSkipList<T> ret_list(*this);
			ret_list += list;
			return ret_list;
		}
		/// @brief thie function will copy all data from other list
		/// @param list other object which type is const MSkipList&
		void operator=(const MSkipList<T>& list)
		{}
		/// @brief thie function will copy all data from other list
		/// @param list other object which type is MSkipLis&&
		void operator=(MSkipList<T>&& list)
		{}
	public: // ��ɾ���
		void push_back(const T& ele)
		{
			// Ϊ�½ڵ�����ڴ�ռ�
			__MSkipListNode__<T>* node = this->__getNewNode__();
			
			// �����½ڵ�
			new(node) __MSkipListNode__<T>(ele, nullptr, this->m_max_level);
			// �޸�β�ڵ�
			this->m_tail->next = node;
			this->m_tail = node;
			for (size_t i = 0; i < node->index_level; ++i)
			{
				this->m_level_tail[i].pointer = node;
			}
			this->m_size += 1;
			
			// ���������㼶
			this->__updateLevel__();

		}
		void pop_back()
		{
			auto tail_node = this->m_tail;

		}
		void push_front(const T& ele)
		{
			// Ϊ�½ڵ�����ڴ�ռ�
			__MSkipListNode__<T>* node = this->__getNewNode__();

			// �����½ڵ�
			new(node) __MSkipListNode__<T>(ele, nullptr, this->m_max_level);

			// �޸�ͷ�ڵ�
			node->next = this->m_header;
			this->m_header = node;
			for (size_t i = 0; i < node->index_level; ++i)
			{
				node->index_next[i] = this->m_level_header[i].pointer;
				this->m_level_header[i].pointer = node;
			}
			this->m_size += 1;

			// ���������㼶
			this->__updateLevel__();
		}
		void pop_front()
		{}
		void emplace_back(T&& ele)
		{
			// Ϊ�½ڵ�����ڴ�ռ�
			__MSkipListNode__<T>* node = this->__getNewNode__();

			// �����½ڵ�
			new(node) __MSkipListNode__<T>(std::move(ele), nullptr, this->m_max_level);

			// �޸�ͷ�ڵ�
			node->next = this->m_header;
			this->m_header = node;
			for (size_t i = 0; i < node->index_level; ++i)
			{
				node->index_next[i] = this->m_level_header[i].pointer;
				this->m_level_header[i].pointer = node;
			}
			this->m_size += 1;

			// ���������㼶
			this->__updateLevel__();
		}
		void emplace_front(T&& ele)
		{
			// Ϊ�½ڵ�����ڴ�ռ�
			__MSkipListNode__<T>* node = this->__getNewNode__();

			// �����½ڵ�
			new(node) __MSkipListNode__<T>(ele, nullptr, this->m_max_level);

			// �޸�ͷ�ڵ�
			node->next = this->m_header;
			this->m_header = node;
			for (size_t i = 0; i < node->index_level; ++i)
			{
				node->index_next[i] = this->m_level_header[i].pointer;
				this->m_level_header[i].pointer = node;
			}
			this->m_size += 1;

			// ���������㼶
			this->__updateLevel__();
		}
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
	public: // ��ȡ��Ϣ �� ������
		void data()
		{}
		inline size_t size()
		{
			return this->m_size;
		}
		inline size_t length()
		{
			return this->m_size;
		}
		inline size_t capacity()
		{
			return this->m_capacity;
		}
		inline size_t empty()
		{
			return (this->m_size == 0);
		}
		void begin()
		{}
		void end()
		{}
	public: // �����ڴ����
		void resize()
		{}
		void reserve()
		{}
		void clear()
		{}
	private: // ˽�з���	
		__MSkipListNode__<T>* __getNewNode__()
		{
			// Ϊ�½ڵ�����ڴ�ռ�
			__MSkipListNode__<T>* node = nullptr;
			// ���ս����
			if (this->m_null_node_header == nullptr)
			{
				this->m_null_node_header = static_cast<__MSkipListNode__<T>*>\
					(this->m_allocator.allocate(this->m_capacity * __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__));
				this->m_capacity *= __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__;
				// �зַ�����ڴ�
				__MSkipListNode__<T>* tmp_ptr = this->m_null_node_header;
				for (; tmp_ptr != nullptr && tmp_ptr->next != nullptr; tmp_ptr = tmp_ptr->next)
				{
					tmp_ptr->next = tmp_ptr + 1;
				}
				tmp_ptr->next = nullptr;
			}
			node = this->m_null_node_header;
			this->m_null_node_header = this->m_null_node_header->next;
			return node;
		}
		void __updateLevel__()
		{
			// ���������������
			if (this->m_size % 2000 == 0)
			{
				this->m_max_level += 1;
			}
		}

	private:
		MAtomicLock m_atomic_lock; // ԭ����
		__MSkipListNode__<T>* m_header; // ����ͷ
		__MSkipListNode__<T>* m_tail; // ����β
		__MSkipListNode__<T>* m_null_node_header; //���ݵĻ�δʹ�õ�ս����
		MBitmapAllocator<__MSkipListNode__<T>> m_allocator; // ������
		std::vector<__MSkipListIndexBround__<T>> m_level_header; // ����ͷ����
		std::vector<__MSkipListIndexBround__<T>> m_level_tail; // ����β����
		size_t m_size; // Ԫ����
		size_t m_capacity; // ����
		size_t m_max_level; // ��������㼶
	};
}

#endif // !__MUZI_MSKIPLIST_H__