#pragma once
#ifndef __MUZI_MASYNCSERVER_H__
#define __MUZI_MASYNCSERVER_H__
#include"MNet/MNetBase.h"
#include"MAsyncSocket.h"

namespace MUZI::NET::ASYNC
{
	class MAsyncServer : public MAsyncSocket
	{
	public:
		MAsyncServer(MServerEndPoint& endpoint);


	public:
		int listen(int back_log);
		NetAsyncIOAdapt accept(int& ec);


	private:
		TCPAcceptor acceptor;
	};
}




#endif // !__MUZI_MASYNCSERVER_H__
