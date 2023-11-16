#pragma once
#ifndef __MUZI_MVECTOR_H__
#define __MUZI_MVECTOR_H__
#include<compare>
#include<iso646.h>

namespace MUZI
{
	template<typename T>
	class MVector
	{
	public:
		using vaule = T;
		using iterator = MVector<T>::Iterator;
	public:
		class MVectorView
		{

		};
	public:
		class Iterator
		{

		};
	public:
		void operator+=(const MVector<T>&){}
		void operator+(const MVector<T>&){}
		void operator-=(const MVector<T>&){}
		void operator-(const MVector<T>&){}
		void operator*=(const MVector<T>&){}
		void operator*(const MVector<T>&){}
		void operator/=(const MVector<T>&){}
		void operator/(const MVector<T>&){}
		void operator<<(){}
		void operator>>(){}
		void operator=(const MVector<T>&){}
		void operator=(const MVectorView<T>&){}
		void operator~(){}
		T& operator[](size_t index){}
		bool operator==()
		{
			return false;
		}
		std::weak_ordering operator<=>()
		{
			return std::weak_ordering();
		}
	public:
		void pop_back(const T&) {}
		void push_back(const T&)
		{}
		void emplace_back()
		{}
		void insert() {}
		void erase() {}
		void erase_if(){}
		void find(){}
		void assign(){}
		void at() {}
		void clear() {}
		void emplace(){}
		void swap(){}
	public:
		void split(){}
		void filter(){}
		void sort(){}
	public:
		void resize() {}
		void reserve(){}
	public:
		T* data() 
		{
			return this->m_data;
		}
		size_t length()
		{
			return this->size;
		}
		size_t size()
		{
			return this->m_size;
		}
		size_t capacity()
		{
			return this->m_capacity;
		}
		T& front()
		{
			return *this->m_data;
		}
		T& back()
		{
			return *(this->m_data + this->m_size);
		}
		void begin(){}
		void end(){}
		void cbegin(){}
		void cend(){}
		void rbegin(){}
		void rend(){}
		void crbegin(){}
		void crend(){}
		bool empty()
		{
			return this->m_data == nullptr or this->size == 0;
		}

	public:
		T* data() const 
		{
			return this->m_data;
		}
		size_t length() const
		{
			return this->size;
		}
		size_t size() const
		{
			return this->m_size;
		}
		size_t capacity() const
		{
			return this->m_capacity;
		}
		T& front() const
		{
			return *this->m_data;
		}
		T& back() const
		{
			return *(this->m_data + this->m_size);
		}
		void begin() const {}
		void end() const {}
		void cbegin() const {}
		void cend() const {}
		void rbegin() const {}
		void rend() const {}
		void crbegin() const {}
		void crend() const {}
		bool empty() const
		{
			return this->m_data == nullptr or this->size == 0;
		}
		
	private:
		size_t m_size;
		size_t m_capacity;
		T* m_data;
	};
}

#endif // !__MUZI_MVECTOR_H__
