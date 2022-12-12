#pragma
#ifndef __MUZI_MSPAN_H__
#define __MUZI_MSPAN_H__
#include<concepts>
#include<string>
#include<memory>
#include"MIterator.h"
namespace MUZI
{
	template<typename T>
	concept __muzi_span_stl_type__ = requires(T x)
	{
		typename T::iterator;
		typename T::value_type;
		{x.size()}->std::same_as<size_t>;
		{x.begin()}->std::same_as<typename T::iterator>;
		{x.end()}->std::same_as<typename T::iterator>;
		x.operator[];
	}
	&& requires(typename T::iterator x)
	{
		{*x};
		x.operator++();
		x.operator--();
		std::copyable<typename T::iterator>;
		std::movable<typename T::iterator>;
		//std::derived_from<typename T::iterator, MIterator>;
	};

	// 采用value_type提取出类型
	template<__muzi_span_stl_type__ T, 
		std::size_t stl_element_size = 
		sizeof(std::remove_reference<typename T::value_type>::type),
		typename Alloc = std::allocator<typename T::iterator>>
	class MSpan
	{
	public:
		class span_iterator :public MIterator
		{
		public:
			// 这里采用new出来的方式
			span_iterator(T::iterator* item)
			{
				this->item = item;
			}
			~span_iterator()
			{
				delete this->item;
				this->item = nullptr;
			}
		public:
			void operator++()
			{
				this->item->operator++();
			}
			void operator--()
			{
				this->item->operator--();
			}
			bool operator==(MIterator& item)
			{
				// 派生类指针/引用转换，需要基类存在虚函数
				return this->item->operator==(*(dynamic_cast<MSpan::iterator&>(item).item));
			}
			//bool operator<(MIterator& item)
			//{
			//	return this->item->operator<(*(dynamic_cast<MSpan::iterator&>(item).item));
			//}
			//bool operator>(MIterator& item)
			//{
			//	return this->item->operator>(*(dynamic_cast<MSpan::iterator&>(item).item));
			//}
			T::value_type& operator*()
			{
				return this->item->operator*();
			}
		public:
			T::iterator* item;
		};
	public:
		using iterator = span_iterator;
	public:
		MSpan(T& other)
		{
			this->_data = &other;
			this->n = other.size();
		}
		MSpan(MSpan& other)
		{
			this->_data = other._data;
			this->n = other.n;
		}
		~MSpan()
		{
			this->_data = nullptr;
			this->n = 0;
		}
	public:
		inline T* data()
		{
			return this->data;
		}
		// 后置返回类型
		auto operator[](size_t pos)->std::remove_pointer<decltype(this->data())>::type::value_type
		{
			return *(this->_data->operator[](pos));
		}
		constexpr size_t size()
		{
			return this->size;
		}
		constexpr size_t size_bites()
		{
			return this->size * stl_element_size;
		}
		inline bool empty()
		{
			return n == 0;
		}
		inline span_iterator begin()
		{
			return span_iterator(new T::iterator(this->_data->begin()));
		}
		inline span_iterator end()
		{
			return span_iterator(new T::iterator(this->_data->end()));
		}
		
	public:
		T* _data;
		size_t n;
	};

};
#endif // __MUZI_SPAN_H__