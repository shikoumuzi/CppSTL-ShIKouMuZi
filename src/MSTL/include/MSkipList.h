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
		using size_type = size_t;
		using reference = T&;
		using const_reference = const T&;
		using index_t = int;
	private:
		template<typename T>
		friend struct __MSkipListIndexBround__;

	private:
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
				index_level(0),
				index_next(nullptr)
			{}
			__MSkipListNode__(__MSkipListNode__* next, int max_level) :
				value(),
				index_level(this->randLevel(max_level)),
				index_next(nullptr)
			{}
			__MSkipListNode__(const T& value, __MSkipListNode__* next, int max_level) :
				value(value),
				index_level(this->randLevel(max_level)),
				index_next(nullptr)
			{}
			__MSkipListNode__(T&& value, __MSkipListNode__* next, int max_level) :
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
				this->index_next = nullptr;
			}
		public:
			inline __MSkipListNode__*& next()
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
			//__MSkipListNode__* next; // ��һ���ڵ�
			__MSkipListNode__** index_next;// ��������һ���ڵ㼯��
			index_t index_level; // ��������
			std::atomic<bool> m_writing_flag; // �����ڲ����������жϵ�ǰ����Ƿ���Ҫ�޸�
		};
		template<typename T = __MDefaultTypeDefine__>
		struct __MSkipListIndexBround__
		{
		public:
			struct MSkipList<T>::__MSkipListNode__* pointer;
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
			__MSkipListNode__* initNode()
			{
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__();
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				return p_ret;
			}
			__MSkipListNode__* initNode(__MSkipListNode__* next, int max_level)
			{
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(next, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__* initNode(const T& value, __MSkipListNode__* next, int max_level)
			{
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(value, next, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__* initNode(T&& value, __MSkipListNode__* next, int max_level)
			{
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(std::move(value), next, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__* initNode(const T& value, int max_level)
			{
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(value, max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__* initNode(const T& value, int max_level, bool isMax)
			{
				if (!isMax)
				{
					return this->initNode(value, max_level);
				}
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(std::move(value), max_level, isMax);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__* initNode(T&& value, int max_level)
			{
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(std::move(value), max_level);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__* initNode(T&& value, int max_level, bool isMax)
			{
				if (!isMax)
				{
					return this->initNode(value, max_level);
				}
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(std::move(value), max_level, isMax);
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
			__MSkipListNode__* initNode(const __MSkipListNode__& node)
			{
				__MSkipListNode__* p_ret = this->__getNewNode__();
				new(p_ret) __MSkipListNode__(std::move(node));
				p_ret->index_next = this->__getNewIndexNextList__(p_ret->index_level);
				p_ret->m_writing_flag = false;
				return p_ret;
			}
		public:
			void releaseNode(__MSkipListNode__* node)
			{
				this->m_node_allocator.deallocate(node);
			}
		public:
			void receiveFromFactory(__MSkipListNodeFactory__<T>&& factory)
			{
				this->m_node_allocator.receive(factory.m_node_allocator);
				this->m_capacity += factory.m_capacity;
			}
		private:
			__MSkipListNode__* __getNewNode__()
			{
				// Ϊ�½ڵ�����ڴ�ռ�
				__MSkipListNode__* node = nullptr;
				// ���ս����
				if (this->m_null_node_header == nullptr)
				{
					this->m_null_node_header = static_cast<__MSkipListNode__*>\
						(this->m_node_allocator.allocate(this->m_capacity * __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__));
					this->m_capacity *= __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__;
					// �зַ�����ڴ�
					__MSkipListNode__* tmp_ptr = this->m_null_node_header;
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
			__MSkipListNode__** __getNewIndexNextList__(size_t index_level)
			{
				return static_cast<__MSkipListNode__**>(this->m_index_allocator.allocate(index_level + 1));
			}
		public:
			void clear()
			{
				this->m_index_allocator.clear();
				this->m_node_allocator.clear();
				this->clearNullNodePool();
				this->m_null_node_header = nullptr;
			}
			void clearNullNodePool()
			{
				for (__MSkipListNode__* it = this->m_null_node_header; it != nullptr; it = it->next())
				{
					this->m_node_allocator.deallocate(it, 1);
				}
				this->m_null_node_header = nullptr;
			}
		public:
			void operator=(const __MSkipListNodeFactory__<T>& factory)
			{
			}
		private:
			MBitmapAllocator<__MSkipListNode__*> m_index_allocator; // ����������
			MBitmapAllocator<__MSkipListNode__> m_node_allocator; // �ڵ������
			__MSkipListNode__* m_null_node_header; //���ݵĻ�δʹ�õ�ս����
			size_t m_capacity; // ����
			index_t& m_max_level; // ��������ȼ�
		};
	public:
		template<typename T = __MDefaultTypeDefine__>
		class MSkipListIterator : public std::iterator<std::forward_iterator_tag, T, size_t, const T*, T>
		{
		public:
			friend class MSkipList<T>;
			//friend class MCIterator<T>;
		public:
			MSkipListIterator() :
				data(nullptr)
			{}
			MSkipListIterator(const MSkipListIterator<T>& iter) :
				data(iter.data)
			{}
			MSkipListIterator(MSkipListIterator<T>&& iter) :
				data(iter.data)
			{
				iter.data = nullptr;
			}
		private:
			MSkipListIterator(__MSkipListNode__* data) :
				data(data)
			{}
		public:
			bool operator==(const MSkipListIterator<T>& iter)
			{
				return this->data == iter.data || this->data->value == iter.data->value;
			}
			std::weak_ordering operator<=>(const MSkipListIterator<T>& iter)
			{
				return this->data->value <=> iter.data->value;
			}
			void operator++()
			{
				if (this->data != nullptr || this->data->next() != nullptr)
				{
					this->data = this->data->next();
				}
			}
			MSkipListIterator<T> operator++(int)
			{
				MSkipListIterator<T> ret_iter = *this;
				this->operator++();
				return ret_iter;
			}
			MSkipListIterator<T> operator+(MSkipListIterator::difference_type offset)
			{
				MSkipListIterator<T> iter = *this;
				while (offset--)
				{
					if (this->data != nullptr || this->data->next() != nullptr)
					{
						this->data = this->data->next();
						continue;
					}
					break;
				}
				return iter;
			}
			void operator+=(MSkipListIterator::difference_type offset)
			{
				while (offset--)
				{
					if (this->data != nullptr || this->data->next() != nullptr)
					{
						this->data = this->data->next();
						continue;
					}
					break;
				}
			}
			void operator=(const MSkipListIterator<T>& iter)
			{
				this->data = iter->data;
			}
			T& operator*()
			{
				return this->data->value;
			}
			T& operator->()
			{
				return this->data->value;
			}
		public:
			bool operator==(const MSkipListIterator<T>& iter) const
			{
				return this->data == iter.data || this->data->value == iter.data->value;
			}
			std::weak_ordering operator<=>(const MSkipListIterator<T>& iter) const
			{
				return this->data->value <=> iter.data->value;
			}
			const T& operator*() const
			{
				return this->data->value;
			}
			const T& operator->() const
			{
				return this->data->value;
			}
			MSkipListIterator<T> operator+(MSkipListIterator::difference_type offset) const
			{
				MSkipListIterator<T> iter = *this;
				while (offset--)
				{
					if (this->data != nullptr || this->data->next() != nullptr)
					{
						this->data = this->data->next();
						continue;
					}
					break;
				}
				return iter;
			}
		private:
			inline __MSkipListNode__* getData()
			{
				return this->data;
			}
		public:
			inline bool isVaild()
			{
				return this->data == nullptr;
			}
			inline bool isVaild() const
			{
				return this->data == nullptr;
			}
		public:
			__MSkipListNode__* data;
		};

		template<typename T = __MDefaultTypeDefine__>
		class MCIterator : private MSkipListIterator<T>
		{
		public:
			friend class MSkipList<T>;
		public:
			MCIterator() {}
			MCIterator(const MCIterator<T>& iter) :
				data(iter.data)
			{}
			MCIterator(MCIterator<T>&& iter) :
				data(iter.data)
			{
				iter.data = nullptr;
			}
			MCIterator(const MSkipListIterator<T>& iter) :
				data(iter.data)
			{}
			MCIterator(MSkipListIterator<T>&& iter) :
				data(iter.data)
			{
				iter.data = nullptr;
			}
		private:
			MCIterator(__MSkipListNode__* data) :
				data(data)
			{}
		public:
			bool operator==(const MCIterator<T>& iter)
			{
				return this->data == iter.data || this->data->value == iter.data->value;
			}
			std::weak_ordering operator<=>(const MCIterator<T>& iter)
			{
				return this->data->value <=> iter.data->value;
			}
			const T& operator*()
			{
				return this->data->value;
			}
			const T& operator->()
			{
				return this->data->value;
			}
		public:
			bool operator==(const MCIterator<T>& iter) const
			{
				return this->data == iter.data || this->data->value == iter.data->value;
			}
			std::weak_ordering operator<=>(const MCIterator<T>& iter) const
			{
				return this->data->value <=> iter.data->value;
			}
			const T& operator*() const
			{
				return this->data->value;
			}
			const T& operator->() const
			{
				return this->data->value;
			}
		};

		//template<typename T>
		//class MRIterator: private MIterator<T>
		//{};
	public:
		template<T>
		using iterator = class MSkipList<T>::MSkipListIterator<T>;
		template<T>
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
			MSkipList()
		{
			this->append(list);
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
			list.m_header = nullptr;
			list.m_tail = nullptr;
			list.m_node_factory.clear();
			list.m_index_header.clear();
			//list.m_level_tail.clear();
			//list.m_level_header.resize(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__, { nullptr });
			//list.m_level_tail.resize(__MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__, { nullptr });
		}
		MSkipList(const std::initializer_list<T>& list) :
			MSkipList()
		{
			this->append(list);
		}
		~MSkipList()
		{
		}
	public:
		void operator+=(const MSkipList<T>& list)
		{
			this->append(list);
		}
		void operator+=(MSkipList<T>&& list)
		{
			this->append(list);
		}
		void operator+=(const std::span<T>& list)
		{
			this->append(list);
		}
		void operator+=(const std::initializer_list<T>& list)
		{
			this->append(list);
		}
		MSkipList<T> operator+(const MSkipList<T>& list)
		{
			MSkipList<T> ret_list = *this;
			ret_list += list;
			return ret_list;
		}
		MSkipList<T> operator+(MSkipList<T>&& list)
		{
			MSkipList<T> ret_list = *this;
			ret_list += list;
			return ret_list;
		}
		MSkipList<T> operator+(const std::span<T>& list)
		{
			MSkipList<T> ret_list = *this;
			ret_list += list;
			return ret_list;
		}
		MSkipList<T> operator+(const std::initializer_list<T>& list)
		{
			MSkipList<T> ret_list = *this;
			ret_list += list;
			return ret_list;
		}
		void operator=(const MSkipList<T>& list)
		{
			this->clear();
			this->append(list);
		}
		void operator=(MSkipList<T>&& list)
		{
			this->clear();
			this->append(list);
		}
		void operator=(const std::span<T>& list)
		{
			this->clear();
			this->append(list);
		}
		void operator=(const std::initializer_list<T>& list)
		{
			this->clear();
			this->append(list);
		}
	public: // ��ɾ���
		void insert(const T& ele)
		{
			// �ȼ������Ƿ���ֵ���еĻ�������ʼ������
			if (this->m_size == 0)
			{
				__MSkipListNode__* node = this->m_node_factory.initNode(ele, this->m_max_level, true);
				this->m_header = node;
				this->m_tail = node;
				this->m_index_header[0].pointer = node;

				return;
			}
			// ���˵eleС��ͷ����ֵ, ��ֱ�Ӳ��뵽ͷ
			if (ele < this->m_header->value)
			{
				__MSkipListNode__* node = this->m_node_factory.initNode(ele, this->m_header, this->m_max_level);
				for (int i = 0; i <= node->index_level; ++i)
				{
					this->m_index_header[0].pointer = node;
				}
				this->m_header = node;
				return;
			}
			__MSkipListNode__* node = this->m_node_factory.initNode(ele, this->m_max_level);
			// ��ݹ��½������߲����������
			// �Ƚ�δ�����Ĳ���ɾѡ�Կ����ҵ���Ҫѡ�������
			__MSkipListNode__* front_node = this->m_index_header[this->m_max_level].pointer;
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
					__MSkipListNode__* front_next_node = front_node->index_next[i - 1];
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
			this->__updateLevel__();
			this->m_size += 1;
		}
		void emplace(T&& ele)
		{
			// �ȼ������Ƿ���ֵ���еĻ�������ʼ������
			if (this->m_size == 0)
			{
				__MSkipListNode__* node = this->m_node_factory.initNode(std::move(ele), this->m_max_level, true);
				this->m_header = node;
				this->m_tail = node;
				this->m_index_header[0].pointer = node;

				return;
			}
			// ���˵eleС��ͷ����ֵ, ��ֱ�Ӳ��뵽ͷ
			if (ele < this->m_header->value)
			{
				__MSkipListNode__* node = this->m_node_factory.initNode(std::move(ele), this->m_header, this->m_max_level);
				for (int i = 0; i <= node->index_level; ++i)
				{
					this->m_index_header[0].pointer = node;
				}
				this->m_header = node;
				return;
			}
			__MSkipListNode__* node = this->m_node_factory.initNode(std::move(ele), this->m_max_level);
			// ��ݹ��½������߲����������
			// �Ƚ�δ�����Ĳ���ɾѡ�Կ����ҵ���Ҫѡ�������
			__MSkipListNode__* front_node = this->m_index_header[this->m_max_level].pointer;
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
					__MSkipListNode__* front_next_node = front_node->index_next[i - 1];
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
			this->__updateLevel__();
			this->m_size += 1;
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
			__MSkipListNode__* front_node = this->m_header;
			__MSkipListNode__* tar_del_node = nullptr;
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
			this->__updateLevel__();
			this->m_node_factory.releaseNode(tar_del_node);
			this->m_size -= 1;
		}
		MSkipListIterator<T> erase(MSkipListIterator<T>& iter)
		{
			if (iter.data == nullptr || iter.data->next() == nullptr)
			{
				return this->end();
			}
			if (iter == this->begin())
			{
				this->pop();
				return this->begin();
			}

			MSkipListIterator<T> ret_node = MSkipListIterator<T>(iter.data->next());
			this->erase(iter.data->value);
			iter.data = nullptr;
			return ret_node;
		}
		void pop()
		{
			for (size_t i = 0; i < this->m_index_header.size(); ++i)
			{
				if (this->m_index_header[i].pointer == this->m_header)
				{
					this->m_index_header[i].pointer = this->m_header->index_next[i];
				}
				else
				{
					break;
				}
			}
		}
		MSkipListIterator<T> find(T value, const MSkipListIterator<T>& it = MSkipListIterator<T>(this->m_header))
		{
			if (it.data == nullptr ||
				this->m_size == 0 ||
				value < it.data->value)
			{
				return this->end();
			}

			size_t i = it.data->index_level;
			__MSkipListNode__* front_node = it.data;
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
					return MSkipListIterator<T>(front_node);
				}
			}
			return MSkipListIterator<T>();
		}
		void append(const MSkipList<T>& list)
		{
			auto end_iter = list.end();
			for (auto it = list.begin(); it != end_iter; ++it)
			{
				this->insert(*it);
			}
		}
		void append(MSkipList<T>&& list)
		{
			this->m_node_factory.receiveFromFactory(std::move(list.m_node_factory));
			// �ȼ������Ƿ���ֵ���еĻ�������ʼ������
			auto end_it = list.end();
			for (auto it = list.begin(); it != end_it; ++it)
			{
				if (this->m_size == 0)
				{
					__MSkipListNode__* node = it.data;
					this->m_header = node;
					this->m_tail = node;
					this->m_index_header[0].pointer = node;

					return;
				}
				// ���˵eleС��ͷ����ֵ, ��ֱ�Ӳ��뵽ͷ
				if (*it < this->m_header->value)
				{
					__MSkipListNode__* node = it.data;
					for (int i = 0; i <= node->index_level; ++i)
					{
						this->m_index_header[0].pointer = node;
					}
					this->m_header = node;
					return;
				}
				__MSkipListNode__* node = it.data;
				// ��ݹ��½������߲����������
				// �Ƚ�δ�����Ĳ���ɾѡ�Կ����ҵ���Ҫѡ�������
				__MSkipListNode__* front_node = this->m_index_header[this->m_max_level].pointer;
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
					if (front_node->index_next[i]->value > *it)
					{
						// ��ǰ�ڵ��½���ѯ
						front_node = front_node->index_next[--i];
						continue;
					}
					if (front_node->index_next[i]->value < *it)
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
					if (front_node->index_next[i]->value >= *it)
					{
						// �������һ�㶼������

						// ��node�ڵ���뵱ǰ��������
						__MSkipListNode__* front_next_node = front_node->index_next[i - 1];
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
					if (front_node->index_next[i]->value < *it)
					{
						front_node = front_node->index_next[i];
						continue;
					}
					//}
				}
				this->__updateLevel__();
				this->m_size += 1;
			}
		}
		void append(const std::span<T>& list)
		{
			auto end_iter = list.end();
			for (auto it = list.begin(); it != end_iter; ++it)
			{
				this->insert(*it);
			}
		}
		void append(const std::initializer_list<T>& list)
		{
			auto end_iter = list.end();
			for (auto it = list.begin(); it != end_iter; ++it)
			{
				this->insert(*it);
			}
		}
		void extend(const MSkipList<T>& list)
		{
			this->append(list);
		}
		void extend(MSkipList<T>&& list)
		{
			this->append(list);
		}
		void extend(const std::span<T>& list)
		{
			this->append(list);
		}
		void extend(const std::initializer_list<T>& list)
		{
			this->append(list);
		}
	public:
		MSkipListIterator<T> find(T value, const MSkipListIterator<T>& it = MSkipListIterator<T>(this->m_header)) const
		{
			if (it.data == nullptr ||
				this->m_size == 0 ||
				value < it.data->value)
			{
				return this->end();
			}

			size_t i = this->m_max_level;
			__MSkipListNode__* front_node = this->m_header;
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
					return MSkipListIterator<T>(front_node);
				}
			}
			return MSkipListIterator<T>();
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
		MSkipListIterator<T> begin()
		{
			return MSkipListIterator<T>(this->m_header);
		}
		MSkipListIterator<T> end()
		{
			return MSkipListIterator<T>();
		}
		/// @brief this function will return iterator which is read only
		/// @return
		MCIterator<T> cbegin()
		{
			return MCIterator<T>(this->m_header);
		}
		MCIterator<T> cend()
		{
			return MCIterator<T>();
		}
		inline T& front()
		{
			return this->m_header->value;
		}
		inline T& back()
		{
			return this->m_tail->value;
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
		MSkipListIterator<T> begin() const
		{
			return MSkipListIterator<T>(this->m_header);
		}
		MSkipListIterator<T> end() const
		{
			return MSkipListIterator<T>();
		}
		MCIterator<T> cbegin() const
		{
			return MCIterator<T>(this->m_header);
		}
		MCIterator<T> cend() const
		{
			return MCIterator<T>();
		}
		inline const T& front() const
		{
			return this->m_header->value;
		}
		inline const T& back() const
		{
			return this->m_tail->value;
		}
	public:
		// �����ڴ����
		//void resize(size_t size)
		//{
		//	if (this->m_size > size)
		//	{
		//		return;
		//	}
		//	else if (this->m_size < size)
		//	{
		//		size_t i = 0;
		//		__MSkipListNode__* node = this->m_header;
		//		for (; node != nullptr; node = node->next())
		//		{
		//			if (i < size)
		//			{
		//				++i;
		//			}
		//			else
		//			{
		//				break;
		//			}
		//		}
		//		return;
		//	}
		//	return;
		//}

		//void reserve(size_t size)
		//{
		//	if (size > this->m_size)
		//	{
		//	}
		//	else
		//	{
		//		this->m_node_factory.clearNullNodePool();
		//	}
		//}
		void clear()
		{
			this->m_size = 0;
			this->m_max_level = 0;
			this->m_header = nullptr;
			this->m_tail = nullptr;
			this->m_node_factory.clear();
			this->m_index_header.clear();
		}
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
				this->m_index_header.push_back(__MSkipListIndexBround__<T>{nullptr});
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
				//__MSkipListNode__* tmp_index_ptr = this->m_level_header[i].pointer;
				__MSkipListNode__* tmp_ptr = this->m_header;
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
		__MSkipListNode__* __findLocationByValue__(const T& value, __MSkipListNode__* start_node = nullptr)
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
			__MSkipListNode__* front_node = this->m_header;
			// ��size ��Ϊ0ʱ һ������һ���ڵ�, Ѱ�ҽڵ�
			for (; i >= 0;)
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
		__MSkipListNode__* m_header; // ����ͷ
		__MSkipListNode__* m_tail; // ����β
		std::vector<__MSkipListIndexBround__<T>> m_index_header; // ����ͷ����
		__MSkipListNodeFactory__<T> m_node_factory;
		size_t m_size; // Ԫ����
		index_t m_max_level; // ��������㼶(�� 0 ��ʼ����)
	};
}

#endif // !__MUZI_MSKIPLIST_H__
