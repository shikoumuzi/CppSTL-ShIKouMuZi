#pragma once
#ifndef __MUZI_MVECTOR_H__
#define __MUZI_MVECTOR_H__
#include<compare>
namespace MUZI
{
	template<typename T>
	class MVector
	{
	public:
		template<typename T>
		class MVectorView
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
		void pop_back() {}
		void push_back()
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
		void front(){}
		void back(){}
		void begin(){}
		void end(){}
		void cbegin(){}
		void cend(){}
		void rbegin(){}
		void rend(){}
		void crbegin(){}
		void crend(){}
		bool empty(){}
		
	private:
		size_t m_size;
		size_t m_capacity;
		T* m_data;
	};
}

#endif // !__MUZI_MVECTOR_H__
