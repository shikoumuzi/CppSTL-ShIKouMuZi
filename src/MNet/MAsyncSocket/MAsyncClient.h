#pragma once
#ifndef __MUZI_MASYNCCLIENT_H__
#define __MUZI_MASYNCCLIENT_H__

#include"MNet/MNetBase.h"
#include"MNet/MEndPoint/MClientEndPoint.h"
#include"MNet/MAsyncSocket/MAsyncSocket.h"

namespace MUZI::NET::ASYNC
{
	class MAsyncClient: public MAsyncSocket
	{
	public:
		MAsyncClient(MClientEndPoint& endpoint);

	public:
		NetAsyncIOAdapt connect(int& error_code, const MServerEndPoint& endpoint);

		

	};



}


#endif // !__MUZI_MASYNCCLIENT_H__
