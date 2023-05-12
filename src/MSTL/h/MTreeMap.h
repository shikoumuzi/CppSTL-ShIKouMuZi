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
	template<typename K, typename V>
		requires std::totally_ordered<K>
	class MTreeMap 
	{
	public:
		MTreeMap(){}
		MTreeMap(const MTreeMap& that) = delete;
		MTreeMap(MTreeMap<K, V>&& that)
		{
			this->tree = new MRBTree< MMapPair<K, V> >(std::move(*that.tree));
		}
	public:
		template<typename K, typename V>
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
			std::weak_ordering operator<=>(const MMapPair<K,V>& that)
			{
				if (this->key > that.key) return std::weak_ordering::greater;
				if (this->key < that.key) return std::weak_ordering::less;
				return std::weak_ordering::equivalent;
			}
			bool operator==(const MMapPair<K,V>& that)
			{
				return (*this <=> that) == 0;
			}
		};
	public:
		class Iterator
		{
		public:
			Iterator(){}
		public:
			void operator++()
			{

			}
			void operator--()
			{

			}

		private:
			
		};
	private:
		MRBTree<MMapPair<K, V>>* tree;
	public:
		// …Ë÷√map
		void set(K& key, V& value)
		{
			MMapPair<K, V> pair(key, value);
			this->tree->set(pair, pair);
		}
		const V& get(K& key)
		{
			const MMapPair<K, V>* result;
			return ((result = this->tree->find(MMapPair<K, V>(key))) != nullptr) ? result->value : MMapPair<K, V>::null;
		}
		const V get(K& key) const
		{
			const MMapPair<K, V>* result;
			return ((result = this->tree->find(MMapPair<K, V>(key))) != nullptr) ? result->value : MMapPair<K, V>::null;
		}
		void inset(K& key, V& value)
		{
			this->tree->insert(std::forward<MMapPair<K, V>>(MMapPair(key, value)));
		}
		const V& operator[](K& key)
		{
			return this->get(key);
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
