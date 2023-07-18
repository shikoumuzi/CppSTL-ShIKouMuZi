#include "MAsyncServer.h"

namespace MUZI::NET::ASYNC
{
	class MAsyncServer::MAsyncServerData
	{
	public:
		MAsyncServerData(MAsyncServer* parent, IOContext& io_context): parent(parent), acceptor(io_context)
		{}
	public:
		MAsyncServer* parent;
		TCPAcceptor acceptor;
		std::map<std::string, NetAsyncIOAdapt> sessions;
	};


	MAsyncServer::MAsyncServer(int& error_code, const MServerEndPoint& endpoint) 
		:MAsyncSocket(),
		m_data(new MAsyncServerData(this, this->getIOContext()))
		
	{
		EC ec;
		
		this->m_data->acceptor.bind(*endpoint.getEndPoint(error_code), ec);
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer", "Bind Error Code is %d, Error Message is %s", MERROR::BIND_ERROR, ec.message().c_str());
			error_code = MERROR::BIND_ERROR;
		}
	}

	int MAsyncServer::listen(int back_log)
	{
		this->m_data->acceptor.listen(back_log);
		return 0;
	}

	NetAsyncIOAdapt MAsyncServer::accept(int& error_code)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		this->m_data->sessions.emplace(adapt->getUUID(), adapt);
		this->m_data->acceptor.async_accept(adapt->socket, 
			[this, adapt](const EC& ec)->void 
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

	int MAsyncServer::accept(NetAsyncIOAdapt adapt)
	{
		EC ec;
		this->m_data->acceptor.async_accept(adapt->socket,
			[this, adapt](const EC& ec)->void
			{
				if (this->handle_accpet(adapt, ec) == 0) {
					this->accept(adapt);
				}
			});
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer", "Bind Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message().c_str());
			return MERROR::ACCEPT_ERROR;
		}
		return 0;
	}

	std::map<String, NetAsyncIOAdapt>& MAsyncServer::getSessions()
	{
		return this->m_data->sessions;
	}


	int MAsyncServer::handle_accpet(NetAsyncIOAdapt adapt, const EC& ec)
	{
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer::handle_accpet", "async_handle error, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message());
			// 置空以放开连接
			this->m_data->sessions.erase(adapt->getUUID());
			return MERROR::ACCEPT_ERROR;
		}
		return 0;
	}

}