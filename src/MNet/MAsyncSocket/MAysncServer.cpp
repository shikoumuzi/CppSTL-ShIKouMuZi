#include "MAsyncServer.h"
#include"MAsyncServer.h"

namespace MUZI::NET::ASYNC
{
	MAsyncServer::MAsyncServer(int& error_code, const MServerEndPoint& endpoint) :MAsyncSocket(), acceptor(this->getIOContext())
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
		this->acceptor.async_accept(adapt->socket, 
			[this, &adapt](const EC& ec)->void 
			{
				int lambda_errorcode;
				if (this->handle_accpet(adapt, ec) == 0) {
					this->accept(lambda_errorcode);
				}
			});
		if (ec.value() != 0)
		{
			error_code = MERROR::ACCEPT_ERROR;
			MLog::w("MAsyncServer", "Bind Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message().c_str());
			return NetAsyncIOAdapt();
		}
		return adapt;
	}

	void MAsyncServer::accept()
	{
	}

	int MAsyncServer::handle_accpet(NetAsyncIOAdapt& adapt, const EC& ec)
	{
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer::handle_accpet", "async_handle error, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message());
			adapt.reset();
			return MERROR::ACCEPT_ERROR;
		}
		return 0;
	}

}