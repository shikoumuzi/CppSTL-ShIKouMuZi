#pragma once
#ifndef __MUZI_MARRAY_H__
#define __MUZI_MARRAY_H__
#include<iterator>
#include"MBase/MObjectBase.h"
#include<array>
#include<compare>
namespace MUZI
{
	template<typename T = __MDefaultTypeDefine__>
	class MArray
	{
	public:
		using element_type = T;
		using value_type = T;
		using data_pointer_type = T*;
	};

	template<typename T = __MDefaultTypeDefine__, size_t ElementSize = 1>
	class MStaskArray : public MArray<T>
	{
	public:
		friend class MStaskArray<T>::MIterator;
	public:
		class MIterator : std::iterator<std::random_access_iterator_tag, T, const T, T*, T&>
		{
		public:
			MIterator() :
				parent(nullptr),
				index(0)
			{
			}
			MIterator(MStaskArray* parent, index_t index) :
				parent(parent),
				index(index)
			{
			}
			MIterator(const MIterator& iter) :
				parent(iter.parent),
				index(iter.index)
			{
			}
			MIterator(MIterator&& iter) :
				parent(iter.parent),
				index(iter.index)
			{
				iter.parent = nullptr;
				iter.index = 0;
			}
		public:
			void operator++()
			{
				this->operator+=(1);
			}
			MIterator operator++(int)
			{
				auto ret_iter = *this;
				this->operator+=(1);
				return ret_iter;
			}
			MIterator operator+(size_t offset)
			{
				auto ret_iter = *this;
				ret_iter += offset;
				return ret_iter;
			}
			void operator+=(size_t offset)
			{
				this->index += offset;
				if (this->index > this->parent->size())
				{
					this->index = this->parent->size();
				}
			}
			void operator--()
			{
				this->operator-=(1);
			}
			MIterator operator--(int)
			{
				auto ret_iter = *this;
				this->operator-=(1);
				return ret_iter;
			}
			MIterator operator-(size_t offset)
			{
				auto ret_iter = *this;
				ret_iter -= offset;
				return ret_iter;
			}
			void operator-=(size_t offset)
			{
				this->index -= offset;
				if (this->index < 0)
				{
					this->index = 0;
				}
			}
			T& operator*()
			{
				return this->parent->m_data[this->index];
			}
			T& operator->()
			{
				return this->parent->m_data[this->index];
			}
			void operator=(const MIterator& iter)
			{
				this->index = iter.index;
				this->parent = iter.parent;
			}
			void operator==(const MIterator& iter)
			{
				return this->index == iter.index;
			}
			std::strong_ordering operator<=>(const MIterator& iter)
			{
				return this->index <=> iter.index;
			}
		public:
			const T& operator*() const
			{
				return this->parent->m_data[this->index];
			}
			const T& operator->() const
			{
				return this->parent->m_data[this->index];
			}
			MIterator operator+(size_t offset) const
			{
				auto ret_iter = *this;
				ret_iter += offset;
				return ret_iter;
			}
			MIterator operator-(int offset) const
			{
				auto ret_iter = *this;
				ret_iter -= offset;
				return ret_iter;
			}
			void operator==(const MIterator& iter) const
			{
				return this->index == iter.index;
			}
			std::strong_ordering operator<=>(const MIterator& iter) const
			{
				return this->index <=> iter.index;
			}
		public:
			MStaskArray* parent;
			index_t index;
		};
		using iterator = MIterator;
	public:
		T& operator[](index_t index)
		{
			return this->m_data[index];
		}
		T& at(index_t index)
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
		MIterator begin()
		{
			return MIterator(this, 0);
		}
		MIterator end()
		{
			return MIterator(this, this->size());
		}
		T* data()
		{
			return this->m_data;
		}
	public:
		const T& operator[](index_t index) const
		{
			return this->m_data[index];
		}
		const T& at(index_t index) const
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
		MIterator begin() const
		{
			return MIterator(this, 0);
		}
		MIterator end() const
		{
			return MIterator(this, this->size());
		}
		const T* data() const
		{
			return this->m_data;
		}
	public:
		constexpr size_t size()
		{
			return ElementSize;
		}
	private:
		T m_data[ElementSize];
	};

	template<typename T = __MDefaultTypeDefine__, size_t ElementSize = 1>
	class MHeapArray : public MArray<T>
	{
	public:
		using value_type = T;
	public:
		class MIterator : std::iterator<std::random_access_iterator_tag, T, const T, T*, T&>
		{
		public:
			MIterator() :
				parent(nullptr),
				index(0)
			{
			}
			MIterator(MHeapArray<T>* parent, index_t index) :
				parent(parent),
				index(index)
			{
			}
			MIterator(const MIterator& iter) :
				parent(iter.parent),
				index(iter.index)
			{
			}
			MIterator(MIterator&& iter) :
				parent(iter.parent),
				index(iter.index)
			{
				iter.parent = nullptr;
				iter.index = 0;
			}
		public:
			void operator++()
			{
				this->operator+=(1);
			}
			MIterator operator++(int)
			{
				auto ret_iter = *this;
				this->operator+=(1);
				return ret_iter;
			}
			MIterator operator+(size_t offset)
			{
				auto ret_iter = *this;
				ret_iter += offset;
				return ret_iter;
			}
			void operator+=(size_t offset)
			{
				this->index += offset;
				if (this->index > this->parent->size())
				{
					this->index = this->parent->size();
				}
			}
			void operator--()
			{
				this->operator-=(1);
			}
			MIterator operator--(int)
			{
				auto ret_iter = *this;
				this->operator-=(1);
				return ret_iter;
			}
			MIterator operator-(size_t offset)
			{
				auto ret_iter = *this;
				ret_iter -= offset;
				return ret_iter;
			}
			void operator-=(size_t offset)
			{
				this->index -= offset;
				if (this->index < 0)
				{
					this->index = 0;
				}
			}
			T& operator*()
			{
				return this->parent->m_data[this->index];
			}
			T& operator->()
			{
				return this->parent->m_data[this->index];
			}
			void operator=(const MIterator& iter)
			{
				this->index = iter.index;
				this->parent = iter.parent;
			}
			void operator==(const MIterator& iter)
			{
				return this->index == iter.index;
			}
			std::strong_ordering operator<=>(const MIterator& iter)
			{
				return this->index <=> iter.index;
			}
		public:
			const T& operator*() const
			{
				return this->parent->m_data[this->index];
			}
			const T& operator->() const
			{
				return this->parent->m_data[this->index];
			}
			MIterator operator+(size_t offset) const
			{
				auto ret_iter = *this;
				ret_iter += offset;
				return ret_iter;
			}
			MIterator operator-(int offset) const
			{
				auto ret_iter = *this;
				ret_iter -= offset;
				return ret_iter;
			}
			void operator==(const MIterator& iter) const
			{
				return this->index == iter.index;
			}
			std::strong_ordering operator<=>(const MIterator& iter) const
			{
				return this->index <=> iter.index;
			}
		public:
			MHeapArray<T>* parent;
			index_t index;
		};
		using iterator = MIterator;
	public:
		friend class MHeapArray<T>::MIterator;
	public:
		MHeapArray() :
			m_data(new T[ElementSize]) {}
		~MHeapArray()
		{
			delete[] this->m_data;
			this->m_data = nullptr;
		}
	public:
		T& operator[](size_t index)
		{
			return this->m_data[index];
		}
		T& at(index_t index)
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
		MIterator begin()
		{
			return MIterator(this, 0);
		}
		MIterator end()
		{
			return MIterator(this, this->size());
		}
		T* data()
		{
			return this->m_data;
		}
	public:
		const T& operator[](size_t index) const
		{
			return this->m_data[index];
		}
		const T& at(index_t index) const
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
		MIterator begin() const
		{
			return MIterator(this, 0);
		}
		MIterator end() const
		{
			return MIterator(this, this->size());
		}
		const T* data() const
		{
			return this->m_data;
		}
	public: 
		constexpr size_t size()
		{
			return ElementSize;
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
