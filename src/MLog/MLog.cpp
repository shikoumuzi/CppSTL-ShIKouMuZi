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
		va_list args;
		va_start(args, msg);
		fprintf(stderr, (tag + ": :" + msg).c_str(), args);
		va_end(args);
	}
	void MLog::s(String tag, String msg, ...)
	{
	}
}