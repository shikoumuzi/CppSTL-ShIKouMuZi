#include"MAsyncClient.h"


namespace MUZI::NET::ASYNC
{
	


	MAsyncClient::MAsyncClient(MClientEndPoint& endpoint)
	{
	}

	NetAsyncIOAdapt MAsyncClient::connect(int& error_code, const MClientEndPoint& endpoint)
	{
		NetAsyncIOAdapt adapt(new Session(TCPSocket(this->getIOContext())));
		int error_code;
		EC ec;
		//adapt->socket.async_connect(endpoint.getEndPoint(error_code), ec);

		return 0;
	}

}