#pragma once
#ifndef __MUZI_MTREEMAP_H__
#define __MUZI_MTREEMAP_H__
#include"MRBTree.h"
#include"MMap.h"
#include<compare>
#include<concepts>
namespace MUZI
{
	template<typename K, typename V>
		requires std::totally_ordered<K>
	class MTreeMap 
	{
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
	private:
		MRBTree<MMapPair<K, V>> tree;
	public:
		// …Ë÷√map
		void set(K key, V value)
		{
			MMapPair<K, V> pair(key, value);
			this->tree.set(pair, pair);
		}
		V get(K key)
		{
			V value;
			return value;
		}
		void inset(K key, V value)
		{
			this->tree.insert(std::forward<MMapPair<K, V>>(MMapPair(key, value)));
		}
		const V& operator[](K key)
		{
			const MMapPair<K,V>* result;
			V null_value;
			return ((result = this->tree.find(MMapPair<K, V>(key))) != nullptr) ? result->value : MMapPair<K, V>::null;
		}
		const MRBTree<MMapPair<K, V>>& data() const 
		{
			return this->tree;
		}
	};

}
#endif // !__MUZI_MTREEMAP_H__
