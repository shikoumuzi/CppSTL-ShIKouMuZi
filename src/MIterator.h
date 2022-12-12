#pragma once
#ifndef __MUZI_MITERATOR_H__
namespace MUZI
{
	class MIterator
	{
	public:
		MIterator()
		{

		}
		~MIterator()
		{

		}
	public:
		virtual void operator++() = 0;
		virtual void operator--() = 0;
	};
};
#endif // !__MUZI_MITERATOR_H__
