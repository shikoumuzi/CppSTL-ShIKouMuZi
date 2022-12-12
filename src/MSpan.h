#pragma
#ifndef __MUZI_MSPAN_H__
#define __MUZI_MSPAN_H__
#include<concepts>
#include<string>
#include"MIterator.h"
namespace MUZI
{
	template<typename T>
	concept __muzi_span_stl_type__ = requires(T x)
	{
		// deriver_from表示派生自某个基类
		{T::iterator}->std::derived_from<MIterator>;
		{x.size()}->std::same_as<size_t>;
		{x.begin()}->std::derived_from<MIterator>;
		{x.end()}->std::derived_from<MIterator>;
	};
	
	
	template<__muzi_span_stl_type__ T>
	class MSpan
	{
	public:
		MSpan()
		{

		}
		MSpan(T other)
		{

		}
		~MSpan()
		{

		}
	public:
		
	};

};
#endif // __MUZI_SPAN_H__