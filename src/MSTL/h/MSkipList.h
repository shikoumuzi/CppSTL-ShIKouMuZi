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
#define __MUZI_MSKIPLIST_DEFAULT_MAX_LEVEL__ 0 /*初始的索引层数*/
#define __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__ 1.5 /*内存申请拓展系数*/
#define __MUZI_MSKIPLIST_DEFAULT_INDEX_LEVEL_UP_COEFFICIENT__ 2047  /*索引层级上升边界值(2048 - 1)*/

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
			T value; // 值
			//__MSkipListNode__<T>* next; // 下一个节点
			__MSkipListNode__<T>** index_next;// 索引和下一个节点集合
			index_t index_level; // 索引层数
			std::atomic<bool> m_writing_flag; // 用以在并发条件下判断当前结点是否需要修改
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
				// 为新节点分配内存空间
				__MSkipListNode__<T>* node = nullptr;
				// 检查战备池
				if (this->m_null_node_header == nullptr)
				{
					this->m_null_node_header = static_cast<__MSkipListNode__<T>*>\
						(this->m_node_allocator.allocate(this->m_capacity * __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__));
					this->m_capacity *= __MUZI_MSKIPLIST_DEFAULT_EXTEAND_COEFFICIENT__;
					// 切分分配的内存
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
			MBitmapAllocator<__MSkipListNode__<T>*> m_index_allocator; // 索引分配器
			MBitmapAllocator<__MSkipListNode__<T>> m_node_allocator; // 节点分配器
			__MSkipListNode__<T>* m_null_node_header; //扩容的还未使用的战备池
			size_t m_capacity; // 容量
			index_t& m_max_level; // 最大索引等级
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
			// 构建索引第一层

			//this->m_level_tail[0].pointer = this->m_tail;
			// 构建剩余所有索引
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
			// 构建索引
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

			// 因为跳表为有序集合， 所以可以在上一个node的基础上进一步搜索
			for (; it != this->cend(); ++it)
			{
				// 构造新节点
				node = this->m_node_factory.initNode(*it, this->m_max_level);
				// 插入新节点
				front_node = this->__findLocationByValue__(node->value, last_node);
				front_node->protectWriting();
				node->next() = front_node->next();
				front_node->next() = node;
				// 增加元素数
				this->m_size += 1;
				// 解锁
				front_node->releaseWriting();
				// 获取新的进度
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

			// 切分所获取到的内存空间
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

			// 构建索引第一层
			//this->m_level_header[0].pointer = this->m_header;
			//this->m_level_tail[0].pointer = this->m_tail;
			// 构建剩余所有索引
			this->__constructAllIndex__();
		}
		/// @brief thie function will copy all data from other list
		/// @param list other object which type is MSkipLis&&
		void operator=(MSkipList<T>&& list)
		{
		}
	public: // 增删查改
		void insert(const T& ele)
		{
			// 先检查表内是否无值，有的话单独初始化内容
			if (this->m_size == 0)
			{
				__MSkipListNode__<T>* node = this->m_node_factory.initNode(ele, this->m_max_level, true);
				this->m_header = node;
				this->m_tail = node;
				this->m_index_header[0].pointer = node;


				return;
			}
			// 如果说ele小于头结点的值, 就直接插入到头
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
			// 便递归下降索引边插入更新索引
			// 先将未索引的部分删选以快速找到需要选择的区间
			__MSkipListNode__<T>* front_node = this->m_index_header[this->m_max_level].pointer;
			size_t i = this->m_max_level;
			for (; i > node->index_level; )
			{
				// 当前front_node->value 一定小于 ele
				//if (front_node->value < ele)
				//{
				// 当下个结点为空时，通过continue，更加细化查找节点
				if (front_node->next() == nullptr)
				{
					//// 高于0的都继续continue
					//if (i != 0)
					//{
					//	continue;
					//}
					//// 直接插入尾巴
					// 因为该for循环 最起码 都是 > 0的 所以该步一定为continue
					--i;
					continue;
				}
				//判断当前索引下一级的值
				if (front_node->index_next[i]->value > ele)
				{
					// 当前节点下降查询
					front_node = front_node->index_next[--i];
					continue;
				}
				if (front_node->index_next[i]->value < ele)
				{
					// 就继续向当前级别索引的下一个索引节点前进
					front_node = front_node->index_next[i];
					continue;
				}
				// 如果相等就直接进入插入程序
				break;
				//}
			}

			// 然后开始边搜索边更新索引
			for (; i >= 0; )
			{
				// 当前front_node->value 一定小于 ele
				//if (front_node->value < ele)
				//{
				// 当下个结点为空时，通过continue，更加细化查找节点
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
				//判断当前索引下一级的值
				if (front_node->index_next[i]->value >= ele)
				{
					// 不是最后一层都是索引

					// 将node节点插入当前级别索引
					__MSkipListNode__<T>* front_next_node = front_node->index_next[i - 1];
					front_node->index_next[i - 1] = node;
					node->index_next[i - 1] = front_next_node;

					front_node = front_node->index_next[--i];
					if (i != 0)
					{
						continue;
					}
					break;

					// 到了最后一层就是next节点连接层
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
			// 当size 不为0时 一定存在一个节点, 寻找节点
			for (; i >= 0;)
			{
				// 代表直到末尾也都没有找到值
				if (front_node->index_next[i] == nullptr)
				{
					if (i == 0)
					{
						return;
					}
					// 当本级索引找不到时则下降索引寻找
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
				//// 如果在两个索引之间 则遍历内部索引
				//if (value < p_node->index_next[p_node->index_level - 1]->value
				//	and value > p_node->value)
				//{
				//
				//}
				//// 如果小于下部索引跳转到下一个节点
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
	public: // 获取信息 和 迭代器
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
	public: // 简易内存操作
		void resize()
		{}
		void reserve()
		{}
		void clear()
		{}
	private: // 私有方法
		/// @brief this function will get a new node from 'm_allocator'
		/// @return return an object which type is __MSkipListNode__

		/// @brief update the max level based on m_size
		void __updateLevel__()
		{
			// 更新最大索引层数
			if ((this->m_size & __MUZI_MSKIPLIST_DEFAULT_INDEX_LEVEL_UP_COEFFICIENT__) == 0)
			{
				this->m_max_level += 1;
			}
		}
		/// @brief construct all index
		void __constructAllIndex__()
		{
			// 构建索引第一层
			//this->m_level_header[0].pointer = this->m_header;
			//this->m_level_tail[0].pointer = this->m_tail;
			// 构建剩余所有索引
			// 该循环遍历所有层级
			for (size_t i = 1; i < this->m_max_level; ++i)
			{
				//__MSkipListNode__<T>* tmp_index_ptr = this->m_level_header[i].pointer;
				__MSkipListNode__<T>* tmp_ptr = this->m_header;
				// 该循环扫描所有节点
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
			// 如果找到就返回相等的，如果找不到就找小于该value的节点和大于该value的节点的可插入位置
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
			// 当size 不为0时 一定存在一个节点, 寻找节点
			for (;i >= 0;)
			{
				// 代表直到末尾也都没有找到值
				if (front_node->index_next[i] == nullptr)
				{
					if (i == 0)
					{
						return nullptr;
					}
					// 当本级索引找不到时则下降索引寻找
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
		MAtomicLock m_atomic_lock; // 原子锁
		__MSkipListNode__<T>* m_header; // 数据头
		__MSkipListNode__<T>* m_tail; // 数据尾
		std::vector<__MSkipListIndexBround__<T>> m_index_header; // 索引头集合
		__MSkipListNodeFactory__<T> m_node_factory;
		size_t m_size; // 元素数
		index_t m_max_level; // 最大索引层级(从 0 开始计数)
	};
}

#endif // !__MUZI_MSKIPLIST_H__
