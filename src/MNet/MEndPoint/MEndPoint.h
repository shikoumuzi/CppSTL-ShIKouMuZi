#pragma once
#ifndef __MUZI_MENDPOINT_H__
#define __MUZI_MENDPOINT_H__
#include"MNet/MNetBase.h"

namespace MUZI::NET
{
	__interface IMEndPoint
	{
		virtual EndPoint* getEndPoint(int& error_code) = 0;
	};

}


#endif // !__MUZI_MENDPOINT_H__
