#include"MLog.h"
#include<stdarg.h>
namespace MUZI
{
	void MLog::v(String tag, String m_msg, ...)
	{
	}
	void MLog::d(String tag, String m_msg, ...)
	{
		
	}
	void MLog::w(String tag, String m_msg, ...)
	{
		char buffer[1024]{ '\0' };
		va_list args;
		va_start(args, m_msg);
		vsprintf(buffer, (tag + ": :" + m_msg).c_str(), args);
		fprintf(stderr, buffer);
		va_end(args);
	}
	void MLog::s(String tag, String m_msg, ...)
	{
	}
}