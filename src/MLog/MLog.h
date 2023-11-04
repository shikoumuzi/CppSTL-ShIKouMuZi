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
		static void v(String tag, String m_msg, ...);
		static void d(String tag, String m_msg, ...);
		static void w(String tag, String m_msg, ...);
		static void s(String tag, String m_msg, ...);
	};

}
#endif // !__MUZI_MLOG_H__
