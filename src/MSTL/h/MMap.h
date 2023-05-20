#pragma once
#ifndef __MUZI_MMAP_H__
#include<compare>
#include<concepts>
#include"MRBTree.h"
namespace MUZI
{
	//pair
	template<typename T>
	concept __MMap_Pair_First_Ele_Type__ = requires(T x)
	{
		std::totally_ordered<T>;
	};

	template<__MMap_Pair_First_Ele_Type__  K, typename V>
	struct __MMapPair__
	{
	public:
		K key;
		V value;
	public:
		std::weak_ordering operator<=>(const __MMapPair__<K, V>& that)
		{
			if (this->key > that.key) return std::weak_ordering::greater;
			if (this->key < that.key) return std::weak_ordering::less;
			return std::weak_ordering::equivalent;
		}
		bool operator==(const __MMapPair__<K, V>& that)
		{
			return (*this <=> that) == 0;
		}

	};

	//mmap
	template<typename T, typename K, typename V>
	concept __MMAP_TYPE__ = requires(T x, K k, V v)
	{
		{T(std::move(x))} noexcept;

		// …Ë÷√map
		{x.set(k, v)} -> std::same_as<void>;
		{x.get(k)};
		{x.inset(k, v)} -> std::same_as<void>;
		{x.operator[](k)};
		{x.data()};

		// …Ë÷√k
		{K()};
		std::totally_ordered<K>;

		//…Ë÷√V
		{V()};


	};

	template<typename K>
	concept __MMAP_KEY__ = requires(K k)
	{
		{K()};
		std::totally_ordered<K>;
	};

	template<typename V>
	concept __MMAP_VALUE__ = requires(V v1, V v2)
	{
		{V()};
		{v1 = v2};
	};


	template<__MMAP_KEY__ K, __MMAP_VALUE__ V, __MMAP_TYPE__<K, V> Map>
	class MMap
	{
	public:
		MMap(){}
		MMap(const MMap&) = delete;
		MMap(MMap&& map) :map(std::move(map)) {}
	private:
		Map map;
	};

};
#endif // !__MUZI_MMAP_H__

