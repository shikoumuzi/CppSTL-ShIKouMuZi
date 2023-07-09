#pragma once 
#ifndef __MUZI_MLOG_H__
#define __MUZI_MLOG_H__
#include<stdio.h>
#include<string.h>
#include<string>
namespace MUZI
{
	class MLog
	{
	public:
		using String = std::string;
	public:
		static void v(String tag, String msg, ...);
		static void d(String tag, String msg, ...);
		static void w(String tag, String msg, ...);
		static void s(String tag, String msg, ...);
	};

}
#endif // !__MUZI_MLOG_H__
