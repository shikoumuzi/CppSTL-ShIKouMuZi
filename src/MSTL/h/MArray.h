#pragma once
#ifndef __MUZI_MARRAY_H__
#define __MUZI_MARRAY_H__
#include<iterator>
#include"MBase/MObjectBase.h"
#include<array>
namespace MUZI
{
	template<typename T = __MDefaultTypeDefine__>
	class MArray
	{
	public:
		using element_type = T;
		using data_pointer_type = T*;
	};

	template<typename T = __MDefaultTypeDefine__, size_t ElementSize = 1>
	class MStaskArray : MArray<T>
	{
	public:
		class MIterator
		{
		};
	public:
		T& operator[](size_t index)
		{
			return this->m_data[index];
		}
		T& at(size_t index)
		{
			if (index > ElementSize)
			{
				return T();
			}
			return this->m_data[index];
		}
		constexpr T& front()
		{
			return this->m_data[0];
		}
		constexpr T& back()
		{
			return this->m_data[ElementSize - 1];
		}
	public:
		const T& operator[](size_t index) const
		{
			return this->m_data[index];
		}
		const T& at(size_t index) const
		{
			if (index > ElementSize)
			{
				return T();
			}
			return this->m_data[index];
		}
		constexpr const T& front() const
		{
			return this->m_data[0];
		}
		constexpr const T& back() const
		{
			return this->m_data[ElementSize - 1];
		}
	public:

	public:
		constexpr size_t size()
		{
			return ElementSize;
		}
	private:
		T m_data[ElementSize];
	};

	template<typename T = __MDefaultTypeDefine__, size_t ElementSize = 1>
	class MHeapArray : MArray<T>
	{
	public:
		MHeapArray() :
			m_data(new T[ElementSize]),
			m_size(ElementSize) {}
		~MHeapArray()
		{
			delete[] this->m_data;
			this->m_data = nullptr;
		}
	private:
		T* m_data;
	};

	template<typename T = __MDefaultTypeDefine__, size_t ElementSize = 1>
	class MUnInitHeapArray : public MHeapArray<char[sizeof(T)], ElementSize>
	{
	public:
		using element_type = char[sizeof(T)];
	};
}

#endif // !__MUZI_MARRAY_H__
