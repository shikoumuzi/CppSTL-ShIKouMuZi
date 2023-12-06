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
#include<functional>
#include<compare>
#include<unordered_map>
#include"MBase/MObjectBase.h"
#define __MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__ 0 /*��ʼ����������*/
#define __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__ 1.5 /*�ڴ�������չϵ��*/
#define __MUZI_MSKIPLIST_DEFAULT_INDEX_LEVEL_UP_COEFFICIENT__ 2047  /*�����㼶�����߽�ֵ(2048 - 1)*/

namespace MUZI
{
	template<typename T = __MDefaultTypeDefine__>
	class MSkipList
	{
	public:
		using value_type = T;
		using index_t = int;
	private:
		template<typename T = __MDefaultTypeDefine__>
		struct __MSkipListNode__
		{
		public:
			friend class MSkipList<T>;
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
				index_level(this->randLevel(max_level)),
				index_next(nullptr)
			{}
			__MSkipListNode__(const T& value, __MSkipListNode__<T>* next, int max_level) :
				value(value),
				index_level(this->randLevel(max_level)),
				index_next(nullptr)
			{}
			__MSkipListNode__(T&& value, __MSkipListNode__<T>* next, int max_level) :
				value(value),
				index_level(this->randLevel(max_level)),
				index_next(nullptr)
			{}
			__MSkipListNode__(const T& value, int max_level) :
				value(value),
				index_level(this->randLevel(max_level)),
				index_next(nullptr)
			{}
			__MSkipListNode__(const T& value, int max_level, bool isMax) :
				value(value),
				index_level(max_level),
				index_next(nullptr)
			{}
			__MSkipListNode__(T&& value, int max_level) :
				value(value),
				index_level(this->randLevel(max_level)),
				index_next(nullptr)
			{}
			__MSkipListNode__(T&& value, int max_level, bool isMax) :
				value(value),
				index_level(max_level),
				index_next(nullptr)
			{}
			__MSkipListNode__(const __MSkipListNode__& node) :
				value(node.value),
				index_level(node.index_level),
				index_next(nullptr)
			{}
			~__MSkipListNode__()
			{
				this->next = nullptr;
			}
		public:
			inline __MSkipListNode__<T>*& next()
			{
				return this->index_next[0];
			}
		public:
			inline void protectWriting()
			{
				this->m_writing_flag = true;
			}
			inline void releaseWriting()
			{
				this->m_writing_flag = false;
			}
			inline bool isWriting()
			{
				return this->m_writing_flag;
			}
		public:
			T value; // ֵ
			//__MSkipListNode__<T>* next; // ��һ���ڵ�
			__MSkipListNode__<T>** index_next;// ��������һ���ڵ㼯��
			index_t index_level; // ��������
			std::atomic<bool> m_writing_flag; // �����ڲ����������жϵ�ǰ����Ƿ���Ҫ�޸�
		};
		template<typename T = __MDefaultTypeDefine__>
		struct __MSkipListIndexBround__
		{
			struct MSkipList<T>::__MSkipListNode__<T>* pointer;
		};
		template<typename T = __MDefaultTypeDefine__>
		class __MSkipListNodeFactory__
		{
		public:
			friend class MSkipList<T>;
		public:
			__MSkipListNodeFactory__(index_t& max_index, size_t capacity = 0) :
				m_max_level(max_index),
				m_capacity(capacity)
			{}
		public:
			__MSkipListNode__<T>* initNode()
			{
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>();
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(__MSkipListNode__<T>* next, int max_level)
			{
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(next, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(const T& value, __MSkipListNode__<T>* next, int max_level)
			{
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(value, next, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(T&& value, __MSkipListNode__<T>* next, int max_level)
			{
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(std::move(value), next, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(const T& value, int max_level)
			{
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(value, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(const T& value, int max_level, bool isMax)
			{
				if (!isMax)
				{
					return this->initNode(value, max_level);
				}
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(std::move(value), max_level, isMax);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(T&& value, int max_level)
			{
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(std::move(value), max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(T&& value, int max_level, bool isMax)
			{
				if (!isMax)
				{
					return this->initNode(value, max_level);
				}
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(std::move(value), max_level, isMax);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__<T>* initNode(const __MSkipListNode__<T>& node)
			{
				__MSkipListNode__<T>* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__<T>(std::move(value));
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
		public:
			void releaseNode(__MSkipListNode__<T>* node)
			{
				this->m_node_allocator.deallocate(node);
			}
		private:
			__MSkipListNode__<T>* __getNewNode__()
			{
				// Ϊ�½ڵ�����ڴ�ռ�
				__MSkipListNode__<T>* node = nullptr;
				// ���ս����
				if (this->m_null_node_header == nullptr)
				{
					this->m_null_node_header = static_cast<__MSkipListNode__<T>*>\
						(this->m_node_allocator.allocate(this->m_capacity * __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__));
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
			__MSkipListNode__<T>** __getNewIndexNextList__(size_t index_level)
			{
				return static_cast<__MSkipListNode__<T>**>(this->m_index_allocator.allocate(index_level + 1));
			}
		public:
			void clear()
			{
				this->m_index_allocator.clear();
				this->m_node_allocator.clear();
				this->m_null_node_header = nullptr;
			}
		public:
			void operator=(const __MSkipListNodeFactory__<T>& factory)
			{
			}
		private:
			MBitmapAllocator<__MSkipListNode__<T>*> m_index_allocator; // ����������
			MBitmapAllocator<__MSkipListNode__<T>> m_node_allocator; // �ڵ������
			__MSkipListNode__<T>* m_null_node_header; //���ݵĻ�δʹ�õ�ս����
			size_t m_capacity; // ����
			index_t& m_max_level; // ��������ȼ�
		};
	public:
		template<typename T = __MDefaultTypeDefine__>
		class MIterator : public std::iterator<std::forward_iterator_tag, T, size_t, const T*, T>
		{
		public:
			friend class MSkipList<T>;
		public:
			MIterator()
			{}
		private:
			MIterator(__MSkipListNode__<T>* data) :
				data(data)
			{}
		public:
			bool operator==(const MIterator<T>& iterator)
			{
				return false;
			}
			std::strong_ordering operator<=>(const MIterator<T>& iterator)
			{
				return std::strong_ordering();
			}
			void operator++()
			{}
			void operator++(int)
			{}
			T& operator*()
			{
				return *data;
			}
		public:
			__MSkipListNode__<T>* data;
		};

		template<typename T = __MDefaultTypeDefine__>
		class MCIterator : private MIterator<T>
		{};

		//template<typename T>
		//class MRIterator: private MIterator<T>
		//{};
	public:
		using iterator = class MSkipList<T>::MIterator<T>;
		using const_iterator = class MSkipList<T>::MCIterator<T>;
		//using reverse_iterator = class MSkipList<T>::MRIterator<T>;
	public:
		MSkipList() :
			m_size(0),
			m_header(nullptr),
			m_tail(nullptr),
			m_max_level(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			//m_level_tail(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_index_header(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_node_factory(this->m_max_level)
		{}
		MSkipList(const MSkipList<T>& list) :
			m_max_level(list.m_max_level),
			m_node_factory(this->m_max_level, list.capacity()),
			m_size(list.m_size),
			m_index_header(list.m_index_header)
		{
			// ����������һ��

			//this->m_level_tail[0].pointer = this->m_tail;
			// ����ʣ����������
			this->__constructAllIndex__();
		}
		MSkipList(MSkipList<T>&& list) :
			m_max_level(list.m_max_level),
			m_node_factory(this->m_max_level, list.capacity()),
			m_size(list.m_size),
			//m_level_tail(list.m_level_tail),
			m_index_header(list.m_index_header),
			m_header(list.m_header),
			m_tail(list.m_tail)
		{
			list.m_size = 0;
			list.m_max_level = __MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__;
			list.m_index_header.clear();
			//list.m_level_tail.clear();
			list.m_header = nullptr;
			list.m_tail = nullptr;
			list.m_node_factory.clear();

			list.m_index_header.clear();
			//list.m_level_tail.clear();
			//list.m_level_header.resize(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__, { nullptr });
			//list.m_level_tail.resize(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__, { nullptr });
		}
		MSkipList(int size) :
			m_max_level(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_node_factory(this->m_max_level),
			m_size(size),
			m_index_header(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__)
		{
			// ��������
			this->__constructAllIndex__();
		}
		MSkipList(const std::initializer_list<T>& list) :
			m_max_level(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
			m_node_factory(this->m_max_level),
			m_size(list.size()),
			m_index_header(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__)
			//m_level_tail(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__),
		{
			size_t i = 0;
			for (auto it = list.begin(); it != list.end() - 1; ++it)
			{
				(this->m_header + i)->next() = (this->m_header + i + 1);
				new(&(this->m_header->value)) T(*it);
				(this->m_header + i)->index_level = __MSkipListNode__<T>::randLevel(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__);
				i += 1;
			}
			this->m_tail = this->m_header + i;
			this->m_tail->next() = nullptr;

			this->__constructAllIndex__();
		}
		~MSkipList()
		{
		}
	public:
		/// @brief this function will copy all nodes(data) from other list
		/// @param list other object which type is MSkipList&
		void operator+=(const MSkipList<T>& list)
		{
			auto it = list.cbegin();
			__MSkipListNode__<T>* node = this->m_node_factory.initNode(*it, this->m_max_level);
			__MSkipListNode__<T>* front_node = this->__findLocationByValue__(node->value);
			front_node->protectWriting();
			node->next() = front_node->next();
			front_node->next() = node;
			this->m_size += 1;
			front_node->releaseWriting();
			++it;
			__MSkipListNode__<T>* last_node = node;

			// ��Ϊ����Ϊ���򼯺ϣ� ���Կ�������һ��node�Ļ����Ͻ�һ������
			for (; it != this->cend(); ++it)
			{
				// �����½ڵ�
				node = this->m_node_factory.initNode(*it, this->m_max_level);
				// �����½ڵ�
				front_node = this->__findLocationByValue__(node->value, last_node);
				front_node->protectWriting();
				node->next() = front_node->next();
				front_node->next() = node;
				// ����Ԫ����
				this->m_size += 1;
				// ����
				front_node->releaseWriting();
				// ��ȡ�µĽ���
				last_node = node;
			}
		}
		/// @brief this function will connect the end of node from 'this' with the front of node from list, and the list will be empty
		/// @param list other object which type is MSkipList&&
		void operator+=(MSkipList<T>&& list)
		{
			for (auto it = list.cbegin(); it != list.cend(); ++it)
			{
				this->insert(*it);
			}
			list.clear();
		}
		/// @brief this function will copy all nodes(data) from other list
		/// @param list other object which type is const std::span&
		void operator+=(const std::span<T>& list)
		{
			for (auto it = list.begin(); it != list.end(); ++it)
			{
				this->insert(*it);
			}
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
		MSkipList<T> operator+(MSkipList<T>&& list)
		{
			MSkipList<T> ret_list(*this);
			ret_list += std::move(list);
			return ret_list;
		}
		/// @brief this function will copy all nodes(data) from other list and return a new object
		/// @param list other object which type is std::span
		MSkipList<T> operator+(std::span<T>& list)
		{
			MSkipList<T> ret_list(*this);
			ret_list += list;
			return ret_list;
		}
		/// @brief thie function will copy all data from other list
		/// @param list other object which type is const MSkipList&
		void operator=(const MSkipList<T>& list)
		{
			this->m_node_factory.clear();
			this->m_size = list.m_size;
			this->m_max_level = list.m_max_level;
			//this->m_level_tail = list.m_level_tail;
			this->m_index_header = list.m_index_header;

			// �з�����ȡ�����ڴ�ռ�
			size_t i = 0;
			__MSkipListNode__<T>* tmp_header = list.m_header;
			for (; tmp_header->next() != nullptr && tmp_header != nullptr; tmp_header = tmp_header->next())
			{
				new(&this->m_header + i) __MSkipListNode__<T>(tmp_header->value, this->m_header + i + 1, this->m_max_level);
				(this->m_header + i)->index_level = tmp_header->index_level;
				i += 1;
			}
			this->m_tail == this->m_header + i;
			(this->m_header + i)->next() = nullptr;

			// ����������һ��
			//this->m_level_header[0].pointer = this->m_header;
			//this->m_level_tail[0].pointer = this->m_tail;
			// ����ʣ����������
			this->__constructAllIndex__();
		}
		/// @brief thie function will copy all data from other list
		/// @param list other object which type is MSkipLis&&
		void operator=(MSkipList<T>&& list)
		{
		}
	public: // ��ɾ���
		void insert(const T& ele)
		{
			// �ȼ������Ƿ���ֵ���еĻ�������ʼ������
			if (this->m_size == 0)
			{
				__MSkipListNode__<T>* node = this->m_node_factory.initNode(ele, this->m_max_level, true);
				this->m_header = node;
				this->m_tail = node;
				this->m_index_header[0].pointer = node;


				return;
			}
			// ���˵eleС��ͷ����ֵ, ��ֱ�Ӳ��뵽ͷ
			if (ele < this->m_header->value)
			{
				__MSkipListNode__<T>* node = this->m_node_factory.initNode(ele, this->m_header, this->m_max_level);
				for (int i = 0; i <= node->index_level; ++i)
				{
					this->m_index_header[0].pointer = node;
					
				}
				this->m_header = node;
				return;
			}
			__MSkipListNode__<T>* node = this->m_node_factory.initNode(ele, this->m_max_level);
			// ��ݹ��½������߲����������
			// �Ƚ�δ�����Ĳ���ɾѡ�Կ����ҵ���Ҫѡ�������
			__MSkipListNode__<T>* front_node = this->m_index_header[this->m_max_level].pointer;
			size_t i = this->m_max_level;
			for (; i > node->index_level; )
			{
				// ��ǰfront_node->value һ��С�� ele
				//if (front_node->value < ele)
				//{
				// ���¸����Ϊ��ʱ��ͨ��continue������ϸ�����ҽڵ�
				if (front_node->next() == nullptr)
				{
					//// ����0�Ķ�����continue
					//if (i != 0)
					//{
					//	continue;
					//}
					//// ֱ�Ӳ���β��
					// ��Ϊ��forѭ�� ������ ���� > 0�� ���Ըò�һ��Ϊcontinue
					--i;
					continue;
				}
				//�жϵ�ǰ������һ����ֵ
				if (front_node->index_next[i]->value > ele)
				{
					// ��ǰ�ڵ��½���ѯ
					front_node = front_node->index_next[--i];
					continue;
				}
				if (front_node->index_next[i]->value < ele)
				{
					// �ͼ�����ǰ������������һ�������ڵ�ǰ��
					front_node = front_node->index_next[i];
					continue;
				}
				// �����Ⱦ�ֱ�ӽ���������
				break;
				//}
			}

			// Ȼ��ʼ�������߸�������
			for (; i >= 0; )
			{
				// ��ǰfront_node->value һ��С�� ele
				//if (front_node->value < ele)
				//{
				// ���¸����Ϊ��ʱ��ͨ��continue������ϸ�����ҽڵ�
				if (front_node->next() == nullptr)
				{
					front_node->index_next[i - 1] = node;
					node->index_next[i - 1] = nullptr;
					if (i == 0)
					{
						this->m_tail = node;
					}
					--i;
					continue;
				}
				//�жϵ�ǰ������һ����ֵ
				if (front_node->index_next[i]->value >= ele)
				{
					// �������һ�㶼������

					// ��node�ڵ���뵱ǰ��������
					__MSkipListNode__<T>* front_next_node = front_node->index_next[i - 1];
					front_node->index_next[i - 1] = node;
					node->index_next[i - 1] = front_next_node;

					front_node = front_node->index_next[--i];
					if (i != 0)
					{
						continue;
					}
					break;

					// �������һ�����next�ڵ����Ӳ�
				}
				if (front_node->index_next[i]->value < ele)
				{
					front_node = front_node->index_next[i];
					continue;
				}
				//}
			}

			this->m_size += 1;
		}
		void emplace(T&& ele)
		{
			//__MSkipListNode__<T>* node = this->m_node_factory.initNode(std::move(ele), this->m_max_level);
			//__MSkipListNode__<T>* front_node = this->__findLocation__(node);
			//node->next = front_node->next;
			//front_node->next = node;
			//this->m_size += 1;
			//front_node->m_writing_flag = false;
		}
		void erase(const T& value)
		{
			if (this->m_size == 0)
			{
				return;
			}
			if (this->m_size == 1)
			{
				return;
			}
			if (value < this->m_header->value)
			{
				return;
			}
			size_t i = this->m_max_level;
			__MSkipListNode__<T>* front_node = this->m_header;
			__MSkipListNode__<T>* tar_del_node = nullptr;
			// ��size ��Ϊ0ʱ һ������һ���ڵ�, Ѱ�ҽڵ�
			for (; i >= 0;)
			{
				// ����ֱ��ĩβҲ��û���ҵ�ֵ
				if (front_node->index_next[i] == nullptr)
				{
					if (i == 0)
					{
						return;
					}
					// �����������Ҳ���ʱ���½�����Ѱ��
					--i;
					continue;
				}
				if (value < front_node->index_next[i]->value)
				{
					front_node = front_node->index_next[--i];
					continue;
				}
				if (value > front_node->index_next[i]->value)
				{
					front_node = front_node->index_next[i];
					continue;
				}
				if (value == front_node->index_next[i]->value)
				{
					front_node->index_next[i] = front_node->index_next[i]->index_next[i];
					tar_del_node = front_node->index_next[i];
					--i;
					continue;
				}
			}
			if (tar_del_node == nullptr)
			{
				return;
			}

			this->m_node_factory.releaseNode(tar_del_node);
			this->m_size -= 1;
		}

		MIterator<T> find(T value, const MIterator<T>& it = MIterator(this->m_index_header[this->m_max_level - 1].pointer))
		{
			__MSkipListNode__<T>* p_node = it.data;
			if (p_node == nullptr)
			{
				return this->end();
			}
			for (;;)
			{
				//// �������������֮�� ������ڲ�����
				//if (value < p_node->index_next[p_node->index_level - 1]->value
				//	and value > p_node->value)
				//{
				//
				//}
				//// ���С���²�������ת����һ���ڵ�
				//else if (value > p_node->index_next[p_node->index_level - 1]->value)
				//{
				//	p_node = p_node->index_next[p_node->index_level - 1];
				//}
				//else
				//{
				//	return this->end();
				//}
			}
		}
		T& find_if(std::function<bool(const T&)>&& func)
		{
			for (__MSkipListNode__<T>* tmp_ptr = this->m_header; tmp_ptr == this->m_tail; tmp_ptr = tmp_ptr->next())
			{
				if (func(tmp_ptr->value))
				{
					return tmp_ptr->value;
				}
			}
		}
		void erase_if()
		{}
	public:
		void find() const
		{}
		T& find_if(std::function<bool(const T&)>&& func) const
		{
			for (__MSkipListNode__<T>* tmp_ptr = this->m_header; tmp_ptr == this->m_tail; tmp_ptr = tmp_ptr->next())
			{
				if (func(tmp_ptr->value))
				{
					return tmp_ptr->value;
				}
			}
		}
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
			return this->m_node_factory.m_capacity;
		}
		inline size_t empty()
		{
			return (this->m_size == 0);
		}
		inline index_t indexLevel()
		{
			return this->m_max_level + 1;
		}
		MIterator<T> begin()
		{
			return MIterator<T>();
		}
		MIterator<T> end()
		{
			return MIterator<T>();
		}
		/// @brief this function will return iterator which is read only
		/// @return
		MIterator<T> cbegin()
		{
			return MIterator<T>();
		}
		MIterator<T> cend()
		{
			return MIterator<T>();
		}
	public:
		inline size_t size() const
		{
			return this->m_size;
		}
		inline size_t length() const
		{
			return this->m_size;
		}
		inline size_t capacity() const
		{
			return this->m_node_factory.m_capacity;
		}
		inline size_t empty() const
		{
			return (this->m_size == 0);
		}
		MIterator<T> cbegin() const
		{
			return MIterator<T>();
		}
		MIterator<T> cend() const
		{
			return MIterator<T>();
		}
	public: // �����ڴ����
		void resize()
		{}
		void reserve()
		{}
		void clear()
		{}
	private: // ˽�з���
		/// @brief this function will get a new node from 'm_allocator'
		/// @return return an object which type is __MSkipListNode__

		/// @brief update the max level based on m_size
		void __updateLevel__()
		{
			// ���������������
			if ((this->m_size & __MUZI_MSKIPLIST_DEFAULT_INDEX_LEVEL_UP_COEFFICIENT__) == 0)
			{
				this->m_max_level += 1;
			}
		}
		/// @brief construct all index
		void __constructAllIndex__()
		{
			// ����������һ��
			//this->m_level_header[0].pointer = this->m_header;
			//this->m_level_tail[0].pointer = this->m_tail;
			// ����ʣ����������
			// ��ѭ���������в㼶
			for (size_t i = 1; i < this->m_max_level; ++i)
			{
				//__MSkipListNode__<T>* tmp_index_ptr = this->m_level_header[i].pointer;
				__MSkipListNode__<T>* tmp_ptr = this->m_header;
				// ��ѭ��ɨ�����нڵ�
				for (; tmp_ptr != nullptr && tmp_ptr->next() != nullptr; tmp_ptr = tmp_ptr->next())
				{
					if (tmp_ptr->index_level != 1 && tmp_ptr->index_level < i + 1)
					{
						//tmp_index_ptr->next = tmp_ptr;
						//tmp_index_ptr = tmp_ptr;
					}
				}
				//this->m_level_tail[i].pointer = tmp_ptr;
			}
		}
		void __constructPartOfIndex__()
		{}
		/// @brief this function will find a location which can insert node orderly and will set writting_flag to be true
		/// @param node a node which be constructed
		/// @return a node which in the front of the target location
		__MSkipListNode__<T>* __findLocationByValue__(const T& value, __MSkipListNode__<T>* start_node = nullptr)
		{
			// ����ҵ��ͷ�����ȵģ�����Ҳ�������С�ڸ�value�Ľڵ�ʹ��ڸ�value�Ľڵ�Ŀɲ���λ��
			if (this->m_size == 0)
			{
				return nullptr;
			}
			if (value < this->m_header->value)
			{
				return nullptr;
			}
			size_t i = this->m_max_level;
			__MSkipListNode__<T>* front_node = this->m_header;
			// ��size ��Ϊ0ʱ һ������һ���ڵ�, Ѱ�ҽڵ�
			for (;i >= 0;)
			{
				// ����ֱ��ĩβҲ��û���ҵ�ֵ
				if (front_node->index_next[i] == nullptr)
				{
					if (i == 0)
					{
						return nullptr;
					}
					// �����������Ҳ���ʱ���½�����Ѱ��
					--i;
					continue;
				}
				if (value < front_node->index_next[i]->value)
				{
					front_node = front_node->index_next[--i];
					continue;
				}
				if (value > front_node->index_next[i]->value)
				{
					front_node = front_node->index_next[i];
					continue;
				}
				if (value == front_node->index_next[i]->value)
				{
					return front_node;
				}
			}
			return nullptr;
		}

	private:
		MAtomicLock m_atomic_lock; // ԭ����
		__MSkipListNode__<T>* m_header; // ����ͷ
		__MSkipListNode__<T>* m_tail; // ����β
		std::vector<__MSkipListIndexBround__<T>> m_index_header; // ����ͷ����
		__MSkipListNodeFactory__<T> m_node_factory;
		size_t m_size; // Ԫ����
		index_t m_max_level; // ��������㼶(�� 0 ��ʼ����)
	};
}

#endif // !__MUZI_MSKIPLIST_H__
