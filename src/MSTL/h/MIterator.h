#pragma once
#ifndef __MUZI_MITERATOR_H__
#define __MUZI_MITERATOR_H__
#include<concepts>
#include<compare>
namespace MUZI
{
	template<typename T, typename U>
	concept __MIterator_Type__ = requires(T x)
	{
		std::totally_ordered<T>;
		{x.operator++()} -> std::same_as<void>;
		{x.operator--()} -> std::same_as<void>;
		{x.operator==()} -> std::same_as<void>;
		{x.operator* ()} -> std::same_as<void>;
		{x.operator+=()} -> std::same_as<void>;
		{x.operator-=()} -> std::same_as<void>;
		{x.operator=()} -> std::same_as<void>;
		{x.data()};
	};

	enum __ITERATOR_STAT__
	{
		ENABLE = 0,
		DISABLE,
		END
	};

	template<typename T, __MIterator_Type__<T> Iterator>
	class MIterator
	{
	public:
		MIterator() = delete;
		MIterator(const MIterator<T, Iterator>& that)
		{
		}
		MIterator(MIterator<T, Iterator>&& that)
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
		inline std::strong_ordering operator<=>(const MIterator& that)
		{
			return this->iterator.operator<=>(that.iterator);
		}
		inline bool operator==(const MIterator& that)
		{
			return this->iterator == that.MIterator;
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
