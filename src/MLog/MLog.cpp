#include"MLog.h"
#include<stdarg.h>
namespace MUZI
{
	void MLog::v(String tag, String msg, ...)
	{
	}
	void MLog::d(String tag, String msg, ...)
	{
		
	}
	void MLog::w(String tag, String msg, ...)
	{
		char buffer[1024]{ '\0' };
		va_list args;
		va_start(args, msg);
		vsprintf(buffer, (tag + ": :" + msg).c_str(), args);
		fprintf(stderr, buffer);
		va_end(args);
	}
	void MLog::s(String tag, String msg, ...)
	{
	}
}