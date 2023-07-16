#include"MAsyncServer.h"

namespace MUZI::NET::ASYNC
{
	MAsyncServer::MAsyncServer(int& error_code, MServerEndPoint& endpoint) :MAsyncSocket(), acceptor(this->getIOContext())
	{
		EC ec;
		
		this->acceptor.bind(*endpoint.getEndPoint(error_code), ec);
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer", "Bind Error Code is %d, Error Message is %s", MERROR::BIND_ERROR, ec.message().c_str());
			error_code = MERROR::BIND_ERROR;
		}
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
			error_code = MERROR::ACCEPT_ERROR;
			MLog::w("MAsyncServer", "Bind Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message().c_str());
			return NetAsyncIOAdapt();

		}
		return adapt;
	}

}