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

	int MAsyncServer::accept(std::function<void(NetAsyncIOAdapt)>& adapt_output)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		
		this->m_data->acceptor.async_accept(adapt->socket, 
			[this, adapt, &adapt_output](const EC& ec)->void
			{
				if (this->handle_accpet(adapt, ec) == 0) {
					adapt_output(adapt);
					this->accept(adapt_output);
				}
			});
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer", "Bind Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message().c_str());
			return MERROR::ACCEPT_ERROR;
		}
		return 0;
	}

	NetAsyncIOAdapt MAsyncServer::accept(int& error_code)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		/*std::make_shared<TCPSocket>(std::move(TCPSocket(this->m_data->io_context, this->m_data->protocol)));*/
		this->m_data->acceptor.accept(adapt->getSocket(), ec);
		if (ec.value() != 0)
		{
			std::cerr << ec.message();
			/*MLog::w("MSyncSocket::accept", "accept is failed, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message().c_str());*/
			error_code = MERROR::ACCEPT_ERROR;
			return NetAsyncIOAdapt();
		}
		return adapt;
	}

	std::map<String, NetAsyncIOAdapt>& MAsyncServer::getNetAsyncIOAdapt()
	{
		return this->m_data->sessions;
	}


	int MAsyncServer::handle_accpet(NetAsyncIOAdapt adapt, const EC& ec)
	{
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer::handle_accpet", "async_handle error, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message());
			return MERROR::ACCEPT_ERROR;
		}
		// 连接成功了才放入池子当中
		this->m_data->sessions.emplace(adapt->getUUID(), adapt);
		return 0;
	}

}