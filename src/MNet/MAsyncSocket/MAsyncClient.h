#pragma once
#ifndef __MUZI_MASYNCCLIENT_H__
#define __MUZI_MASYNCCLIENT_H__

#include"MNet/MNetBase.h"
#include"MNet/MEndPoint/MClientEndPoint.h"
#include"MNet/MAsyncSocket/MAsyncSocket.h"
#include<memory>

namespace MUZI::net::async
{
	class MAsyncClient: public MAsyncSocket
	{
	public:
		MAsyncClient(MClientEndPoint& endpoint);

	public:
		NetAsyncIOAdapt connect(int& error_code, const MClientEndPoint& endpoint);
		void connect();
	public:
		class MAsyncClientData;
	public:
		friend class MAsyncClientData;
	public:
		class MAsyncClientData *m_data;
	};



}


#endif // !__MUZI_MASYNCCLIENT_H__
