#pragma
#ifndef __MUZI_SPAN_H__
#define __MUZI_SPAN_H__
#include<concepts>
#include<string>

namespace MUZI
{
	template<typename T>
	concept __muzi_span_stl_type__ = requires(T x)
	{
		typename T::iterator;
		x.size();
		x.begin();
		x.end();
		/*std::is_same<typename T::iterator, x.begin()>(x.begin());*/
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