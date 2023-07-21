#pragma once
#ifndef __MUZI_MENDPOINT_H__
#define __MUZI_MENDPOINT_H__
#include"MNet/MNetBase.h"

namespace MUZI::net
{
	__interface IMEndPoint
	{
		virtual EndPoint* getEndPoint() = 0;
	};

}


#endif // !__MUZI_MENDPOINT_H__
