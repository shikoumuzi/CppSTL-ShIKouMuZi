#include"MAsyncClient.h"


namespace MUZI::net::async
{

	class MAsyncClient::MAsyncClientData
	{

	};

	MAsyncClient::MAsyncClient(MClientEndPoint& endpoint, NotifiedFunction notified_function)
		:MAsyncSocket(notified_function),
		m_data(new MAsyncClientData())
	{
	}

	NetAsyncIOAdapt MAsyncClient::connect(int& error_code, const MClientEndPoint& endpoint)
	{
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		EC ec;
		//adapt->socket.async_connect(endpoint.getEndPoint(error_code), ec);

		return 0;
	}

	void MAsyncClient::connect()
	{

	}

}