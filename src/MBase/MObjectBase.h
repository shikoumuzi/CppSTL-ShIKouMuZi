#pragma once
#ifndef __MUZI_MOBJECTBASE_H__
#define __MUZI_MOBJECTBASE_H__

#include<iso646.h>
#define CLASSDATA(classname) classname##Data

//#define WINDOWS
#ifdef WINDOWS
#include<windows.h>

#endif
namespace MUZI
{
	using __MDefaultTypeDefine__ = int;
};

#endif // !__MUZI_OBJECTBASE_H__
