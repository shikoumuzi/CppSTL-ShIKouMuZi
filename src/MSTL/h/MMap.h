#pragma once
#ifndef __MUZI_MMAP_H__
#include<compare>
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
	template<typename T>
	concept __MMAP_TYPE__ = requires(T x)
	{
		x.set();
		x.get();
		x.inset();
		x.operator[]();
		x.data();
	};


	template<typename T, typename K, __MMAP_TYPE__ Map = nullptr_t>
	class MMap
	{
	private:
		Map map;
	};

	//hashmap
	template<typename T, typename K>
	class MHashMap
	{

	};

	//treemap
	template<__MMap_Pair_First_Ele_Type__  T, typename K>
	class MTreeMap
	{

	};

};
#endif // !__MUZI_MMAP_H__

