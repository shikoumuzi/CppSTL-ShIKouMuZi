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

	template<__MMap_Pair_First_Ele_Type__  T, typename K>
	struct __MMapPair__
	{
	public:
		T key;
		K value;
	public:
		std::weak_ordering operator<=>(const __MMapPair__& that)
		{
			if (this->key > that.key) return std::weak_ordering::greater;
			if (this->key < that.key) return std::weak_ordering::less;
			return std::weak_ordering::equivalent;
		}
		bool operator==(const __MMapPair__& that)
		{
			return (*this <=> that) == 0;
		}

	};

	//mmap
	template<typename T, typename K, typename V>
	concept __MMAP_TYPE__ = requires(T x, K k, V v)
	{
		// 设置map
		{x.set(k, v)} -> std::same_as<void>;
		{x.get(k)};
		{x.inset(k, v)} -> std::same_as<void>;
		{x.operator[](k)};
		{x.data()};

		// 设置k
		std::totally_ordered<K>;

		//设置V
		{V()} -> std::same_as<V>;

		// 设置内部数据结构之节点数据类型
		typename T::MMapPair;
		{T::MMapPair::key} -> std::same_as<K>;
		{T::MMapPair::value};


	};


	template<typename K, typename V, __MMAP_TYPE__<K, V> Map>
	class MMap
	{
	private:
		Map map;
	};

};
#endif // !__MUZI_MMAP_H__

