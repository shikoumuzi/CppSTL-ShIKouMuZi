#pragma once
#ifndef __MUZI_MITERATOR_H__
#define __MUZI_MITERATOR_H__
#include<concepts>
#include<compare>
namespace MUZI
{
	template<typename T>
	concept __MIterator_Type__ = requires(T x)
	{
		std::totally_ordered<T>;
		x.operator++();  
		x.operator--();
		x.operator<=>();
		x.operator==();
		x.operator* ();
		x.operator+=();
		x.operator-=();
		x.operator=();
		x.data();
	};

	template<__MIterator_Type__ Iterator, typename T>
	class MIterator
	{
	public:
		MIterator() = delete;
		MIterator(const MIterator<Iterator>& that)
		{

		}
		MIterator(MIterator<Iterator>&& that)
		{
		}
	public:
		inline void operator++()
		{
			++this->iterator;
		}
		inline void operator--()
		{
			--this->iterator;
		}
		inline std::strong_ordering:: operator<=>(const MIterator& that)
		{
			return this->iterator.operator<=>(that.iterator);
		}
		inline bool operator==(const MIterator& that)
		{
			return this->iterator == that.iterator;
		}
		inline T operator* ()
		{
			return *this->iterator;
		}
		inline void operator+=(size_t n)
		{
			this->iterator += n;
		}
		inline void operator-=(size_t n)
		{
			this->iterator -= n;
		}
		inline void operator=(const MIterator& that)
		{
			this->iterator = that;
		}
		inline const T* data()
		{
			return iterator.data();
		}
	private:
		Iterator iterator;
	};
};
#endif // !__MUZI_MITERATOR_H__
