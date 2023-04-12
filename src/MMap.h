#pragma once
#ifndef __MUZI_MMAP_H__
#include<compare>
namespace MUZI
{
	template<typename T>
	concept __MMap_Pair_First_Ele_Type__ = requires(T x)
	{
		std::totally_ordered<T>;
	};

	template<__MMap_Pair_First_Ele_Type__  T, typename K>
	struct __MMap_Pair__
	{
	public:
		T key;
		K value;
	public:
		std::weak_ordering operator<=>(const __MMap_Pair__& that)
		{
			if (this->key > that.key) return std::weak_ordering::greater;
			if (this->key < that.key) return std::weak_ordering::less;
			return std::weak_ordering::equivalent;
		}
		bool operator==(const __MMap_Pair__& that)
		{
			return (*this <=> that) == 0;
		}

	};

	template<typename T, typename K>
	class MMap
	{

	};

	template<typename T, typename K>
	class MHashMap: public MMap
	{

	};

	template<__MMap_Pair_First_Ele_Type__  T, typename K>
	class MTreeMap :public MMap
	{

	};

};
#endif // !__MUZI_MMAP_H__

