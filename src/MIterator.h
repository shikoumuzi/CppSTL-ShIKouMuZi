#pragma once
#ifndef __MUZI_MITERATOR_H__
namespace MUZI
{
	class MIterator
	{
	public:
		virtual void operator++() = 0;
		virtual void operator--() = 0;
		virtual bool operator==(MIterator& item) = 0;
		inline virtual bool operator!=(MIterator& item)
		{
			return !this->operator==(item);
		}
		inline virtual bool operator<=(MIterator& item)
		{
			return !this->operator>(item);
		}
		inline virtual bool operator>=(MIterator& item)
		{
			return !this->operator<(item);
		}
		virtual bool operator<(MIterator& item) = 0;
		virtual bool operator>(MIterator& item) = 0;
	};
};
#endif // !__MUZI_MITERATOR_H__
