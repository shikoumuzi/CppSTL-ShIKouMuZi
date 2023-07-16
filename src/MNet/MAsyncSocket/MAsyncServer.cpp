#include"MAsyncServer.h"

namespace MUZI::NET::ASYNC
{
	MAsyncServer::MAsyncServer(MServerEndPoint& endpoint) :MAsyncSocket(), acceptor(this->getIOContext())
	{
		int error_code;
		EC ec;
		
		this->acceptor.bind(*endpoint.getEndPoint(error_code), ec);

	}

	int MAsyncServer::listen(int back_log)
	{
		this->acceptor.listen(back_log);
		return 0;
	}

	NetAsyncIOAdapt MAsyncServer::accept(int& error_code)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new Session(TCPSocket(this->getIOContext())));
		this->acceptor.accept(adapt->socket, ec);
		if (ec.value() != 0)
		{
			return NetAsyncIOAdapt();

		}
		return adapt;
	}

}