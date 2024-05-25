#pragma once
#ifndef __MUZI_MTREEMAP_H__
#define __MUZI_MTREEMAP_H__
#include"MRBTree.h"
#include"MMap.h"
#include"MIterator.h"
#include<compare>
#include<concepts>
namespace MUZI
{
	template<__MMAP_KEY__ K, __MMAP_VALUE__ V>
	class MTreeMap
	{
	public:
		friend class MIterator;
		friend class MReverseIterator;
	public:
		template<__MMAP_KEY__ K, __MMAP_VALUE__ V>
			requires std::totally_ordered<K>
		struct MMapPair
		{
		public:
			static V null;
		public:
			MMapPair() {}
			MMapPair(K key) :key(key) {}
			MMapPair(K key, V value) :key(key), value(value) {}
			MMapPair(const MMapPair<K, V>& that) { memcpy(this, &that, sizeof(MMapPair<K, V>)); }
		public:
			K key;
			V value;

		public:
			std::strong_ordering operator<=>(const MMapPair<K, V>& that)
			{
				if (this->key > that.key) return std::strong_ordering::greater;
				if (this->key < that.key) return std::strong_ordering::less;
				return std::strong_ordering::equivalent;
			}
			bool operator!=(const MMapPair<K, V>& that)
			{
				return (*this <=> that) != 0;
			}
			void operator=(const MMapPair<K, V>& that)
			{
				this->key = that.key;
				this->value = that.value;
			}
		};
	private:
		friend class MIterator;
		using __DATA_TYPE__ = MMapPair<K, V>;
		using __TREE_TYPE__ = MRBTree<__DATA_TYPE__>;
	public:
		using value_type = V;
		using reference = V&;
		using const_reference = const V&;
		using size_type = size_t;
	public:
		MTreeMap()
		{
			this->tree = new MRBTree<MMapPair<K, V>>();
		}
		MTreeMap(const MTreeMap& that) = delete;
		MTreeMap(MTreeMap<K, V>&& that)
		{
			this->tree = that.tree;
			that.tree = nullptr;
		}
		~MTreeMap()
		{
			if (this->tree != nullptr)
			{
				delete this->tree;
			}
		}
	public:
		template<typename K, typename V>
			requires std::totally_ordered<K>
		class MIterator
		{
		public:
			MIterator() :m_it(), m_status(__ITERATOR_STAT__::DISABLE), m_parent(nullptr) {}
			MIterator(MMapPair<K, V>& pair, MTreeMap<K, V>* parent, int status = __ITERATOR_STAT__::ENABLE)
				:m_it(MRBTree<MMapPair<K, V>>::MIterator<MMapPair<K, V>>(pair, parent->tree)), m_parent(parent), m_status(status)
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					this->m_parent = nullptr;
					this->m_status = __ITERATOR_STAT__::DISABLE;
					return;
				}
			}
			MIterator(const MIterator<K, V>& other_it) :m_it(other_it.m_it), m_status(other_it.m_status), m_parent(other_it.m_parent) {}
			MIterator(MIterator<K, V>&& other_it)noexcept :m_it(other_it.m_it), m_status(other_it.m_status), m_parent(other_it.m_parent)
			{
				other_it.m_it.disable();
				other_it.m_parent = nullptr;
				other_it.m_status = __ITERATOR_STAT__::DISABLE;
			}
		public:
			void operator++()
			{
				this->m_it += 1;
			}
			void operator--()
			{
				this->m_it -= 1;
			}
			std::strong_ordering operator<=>(const MIterator<K, V>& that)
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					return std::strong_ordering::equivalent;;
				}
				if (that.m_status == __ITERATOR_STAT__::END)
				{
					if (this->m_status == __ITERATOR_STAT__::END)
					{
						return std::strong_ordering::equivalent;
					}
					else
					{
						return std::strong_ordering::less;
					}
				}
				if (this->m_parent != that.m_parent) return std::strong_ordering::less;// 不是一个就永远小于
				// 从设计上迭代器将从最小的节点开始，有序得返回下一个大小的内容，故直接按照ele元素进行排序
				if (this->m_it >= that.m_it) return std::strong_ordering::greater;
				if (this->m_it < that.m_it) return std::strong_ordering::less;
				return std::strong_ordering::equivalent;
			}
			bool operator==(MIterator<K, V>& it)
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE || this->m_parent != it.m_parent)
				{
					return false;
				}
				return (*this <=> it) == 0;
			}
			const MMapPair<K, V>& operator* ()
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					return MMapPair<K, V>();
				}
				return this->m_it.data()->ele;
				;
			}
			void operator+=(size_t step)
			{
				this->m_it += step;
			}
			void operator-=(size_t step)
			{
				this->m_it -= step;
			}
			inline void operator=(MIterator<K, V>& it)
			{
				this->m_it = it.m_it;
				this->m_status = it.m_status;
				this->m_parent = it.m_parent;
			}
			inline const MMapPair<K, V>* data()
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					return nullptr;
				}
				return &this->m_it.data()->ele;
			}
		public:
			inline int status()
			{
				return this->m_status;
			}

		private:
			MTreeMap<K, V>* m_parent;
			int m_status;
			typename MRBTree<MMapPair<K, V>>::template MIterator<MMapPair<K, V>> m_it;
		};
	public:
		template<typename K, typename V>
			requires std::totally_ordered<K>
		class MReverseIterator
		{
		public:
			MReverseIterator() :m_it(), m_status(__ITERATOR_STAT__::DISABLE), m_parent(nullptr) {}
			MReverseIterator(MMapPair<K, V>& pair, MTreeMap<K, V>* parent, int status = __ITERATOR_STAT__::ENABLE)
				:m_it(MRBTree<MMapPair<K, V>>::MReverseIterator<MMapPair<K, V>>(pair, parent->tree)), m_parent(parent), m_status(status)
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					this->m_parent = nullptr;
					this->m_status = __ITERATOR_STAT__::DISABLE;
					return;
				}
			}
			MReverseIterator(const MReverseIterator<K, V>& other_it) :m_it(other_it.m_it), m_status(other_it.m_status), m_parent(other_it.m_parent) {}
			MReverseIterator(MReverseIterator<K, V>&& other_it)noexcept :m_it(other_it.m_it), m_status(other_it.m_status), m_parent(other_it.m_parent)
			{
				other_it.m_it.disable();
				other_it.m_parent = nullptr;
				other_it.m_status = __ITERATOR_STAT__::DISABLE;
			}
		public:
			void operator++()
			{
				this->m_it += 1;
			}
			void operator--()
			{
				this->m_it -= 1;
			}
			std::strong_ordering operator<=>(const MReverseIterator<K, V>& that)
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					return std::strong_ordering::equivalent;;
				}
				if (that.m_status == __ITERATOR_STAT__::END)
				{
					if (this->m_status == __ITERATOR_STAT__::END)
					{
						return std::strong_ordering::equivalent;
					}
					else
					{
						return std::strong_ordering::less;
					}
				}
				if (this->m_parent != that.m_parent) return std::strong_ordering::less;// 不是一个就永远小于
				// 从设计上迭代器将从最小的节点开始，有序得返回下一个大小的内容，故直接按照ele元素进行排序
				if (this->m_it < that.m_it) return std::strong_ordering::greater;
				if (this->m_it >= that.m_it) return std::strong_ordering::less;
				return std::strong_ordering::equivalent;
			}
			bool operator==(MReverseIterator<K, V>& it)
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE || this->m_parent != it.m_parent)
				{
					return false;
				}
				return (*this <=> it) == 0;
			}
			const MMapPair<K, V>& operator* ()
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					return MMapPair<K, V>();
				}
				return this->m_it.data()->ele;
				;
			}
			void operator+=(size_t step)
			{
				this->m_it += step;
			}
			void operator-=(size_t step)
			{
				this->m_it -= step;
			}
			inline void operator=(MIterator<K, V>& it)
			{
				this->m_it = it.m_it;
				this->m_status = it.m_status;
				this->m_parent = it.m_parent;
			}
			inline const MMapPair<K, V>* data()
			{
				if (this->m_status == __ITERATOR_STAT__::DISABLE)
				{
					return nullptr;
				}
				return &this->m_it.data()->ele;
			}
		public:
			inline int status()
			{
				return this->m_status;
			}

		private:
			MTreeMap<K, V>* m_parent;
			int m_status;
			typename MRBTree<MMapPair<K, V>>::template MReverseIterator<MMapPair<K, V>> m_it;
		};
	private:
		MRBTree<MMapPair<K, V>>* tree;
	public:
		// 设置map
		void set(K& key, V& value)
		{
			MMapPair<K, V> pair(key, value);
			this->tree->set(pair, pair);
		}
		V& get(K& key)
		{
			MMapPair<K, V>* result;
			return ((result = this->tree->find(MMapPair<K, V>(key))) != nullptr) ? result->value : MMapPair<K, V>::null;
		}
		const V get(K& key) const
		{
			const MMapPair<K, V>* result;
			return ((result = this->tree->find(MMapPair<K, V>(key))) != nullptr) ? result->value : MMapPair<K, V>::null;
		}
		void insert(K& key, V& value)
		{
			this->tree->insert(std::forward<MMapPair<K, V>>(MMapPair(key, value)));
		}
		V& operator[](K& key)
		{
			V tmp_v;
			if ((tmp_v = this->get(key)) == MMapPair<K, V>::null)
			{
				tmp_v = V();
				this->insert(key, tmp_v);
			}
			else
			{
				return tmp_v;
			}
		}
		const MRBTree<MMapPair<K, V>>& data() const
		{
			return *this->tree;
		}
	};

	template<typename K, typename V>
	static MMap<K, V, MTreeMap<K, V>>* MTreeMap2MMap()
	{
		return new MMap<K, V, MTreeMap2MMap<K, V>>;
	}

	template<typename K, typename V>
	static MMap<K, V, MTreeMap<K, V>>* MTreeMap2MMap(MTreeMap<K, V>&& map)
	{
		return new MMap<K, V, MTreeMap2MMap<K, V>>(map);
	}
}
#endif // !__MUZI_MTREEMAP_H__
