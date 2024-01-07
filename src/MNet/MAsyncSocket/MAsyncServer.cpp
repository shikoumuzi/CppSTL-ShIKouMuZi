#include "MAsyncServer.h"
#include<thread>
#include<mutex>
#include<atomic>

namespace MUZI::net::async
{
	class MAsyncServer::MAsyncServerData
	{
	public:
		using AnalyzedHeader = MMsgNodeDataBaseMsg(*)(MRecvMsgNode&);

	public:
		MAsyncServerData(MAsyncServer* parent, IOContext& io_context, const MServerEndPoint& endpoint)
			:parent(parent), acceptor(io_context, *endpoint.getEndPoint())
		{}
	public:
		int accpetCallback(NetAsyncIOAdapt adapt, const EC& ec)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::handle_accpet", "async_handle error, Error Code is %d, Error Message is %s", MERROR::MNET_ACCEPT_ERROR, ec.message());
				return MERROR::MNET_ACCEPT_ERROR;
			}
			// 连接成功了才放入池子当中
			this->parent->getSessions().emplace(adapt->getUUID(), adapt);
			//this->parent->readFromSocket(adapt);
			return 0;
		}

	public:
		TCPAcceptor acceptor;
		MAsyncServer* parent;
	};

	MAsyncServer::MAsyncServer(int& error_code, const MServerEndPoint& endpoint, NotifiedFunction notified_fun)
		:MAsyncSocket(notified_fun), m_data(new MAsyncServerData(this, this->getIOContext(), endpoint))
	{}

	MAsyncServer::MAsyncServer(int& error_code, IOContext& context, const MServerEndPoint& endpoint, NotifiedFunction notified_fun)
		:MAsyncSocket(context, notified_fun), m_data(new MAsyncServerData(this, this->getIOContext(), endpoint))
	{
	}

	MAsyncServer::~MAsyncServer()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	int MAsyncServer::listen(int back_log)
	{
		EC ec;
		this->m_data->acceptor.listen(back_log, ec);
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer::listen", "listen is error, Error Code is %d, Error Messahe is %s\n", MERROR::MNET_LISTEN_ERROR, ec.message().c_str());
			return MERROR::MNET_LISTEN_ERROR;
		}
		return 0;
	}

	int MAsyncServer::accept(const AcceptCallBack& adapt_output)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(this->getIOContext()));

		this->m_data->acceptor.async_accept(adapt->socket,
			[this, adapt, adapt_output](const EC& ec)->void
			{
				if (this->m_data->accpetCallback(adapt, ec) == 0) {
					adapt_output(*this, adapt);
					this->accept(adapt_output);
				}
			});
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer::accept", "Bind Error Code is %d, Error Message is %s\n", MERROR::MNET_ACCEPT_ERROR, ec.message().c_str());
			return MERROR::MNET_ACCEPT_ERROR;
		}
		return 0;
	}

	NetAsyncIOAdapt MAsyncServer::accept(int& error_code)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(this->getIOContext()));
		/*std::make_shared<TCPSocket>(std::move(TCPSocket(this->m_data->io_context, this->m_data->protocol)));*/
		this->m_data->acceptor.accept(adapt->getSocket(), ec);
		if (ec.value() != 0)
		{
			std::cerr << ec.message();
			/*MLog::w("MSyncSocket::accept", "accept is failed, Error Code is %d, Error Message is %s", MERROR::MNET_ACCEPT_ERROR, ec.message().c_str());*/
			error_code = MERROR::MNET_ACCEPT_ERROR;
			return NetAsyncIOAdapt();
		}
		return adapt;
	}
}