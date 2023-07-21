#include"MAsyncClient.h"


namespace MUZI::net::async
{
	


	MAsyncClient::MAsyncClient(MClientEndPoint& endpoint)
	{
	}

	NetAsyncIOAdapt MAsyncClient::connect(int& error_code, const MClientEndPoint& endpoint)
	{
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		EC ec;
		//adapt->socket.async_connect(endpoint.getEndPoint(error_code), ec);

		return 0;
	}

}