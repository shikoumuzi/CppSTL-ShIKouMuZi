#include "MAsyncServer.h"

namespace MUZI::net::async
{
	class MAsyncServer::MAsyncServerData
	{
	public:
		MAsyncServerData(MAsyncServer* parent, IOContext& io_context, const MServerEndPoint& endpoint)
			: parent(parent), acceptor(io_context, *endpoint.getEndPoint())
		{}
	public:
		int accpetCallback(NetAsyncIOAdapt adapt, const EC& ec)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::handle_accpet", "async_handle error, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message());
				return MERROR::ACCEPT_ERROR;
			}
			// ���ӳɹ��˲ŷ�����ӵ���
			this->sessions.emplace(adapt->getUUID(), adapt);
			//this->parent->readFromSocket(adapt);
			return 0;
		}
	public:
		MAsyncServer* parent;
		TCPAcceptor acceptor;
		std::map<std::string, NetAsyncIOAdapt> sessions;
	};


	MAsyncServer::MAsyncServer(int& error_code, const MServerEndPoint& endpoint) 
		:m_data(new MAsyncServerData(this, this->getIOContext(), endpoint))
	{}

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
			MLog::w("MAsyncServer::listen", "listen is error, Error Code is %d, Error Messahe is %s\n", MERROR::LISTEN_ERROR, ec.message().c_str());
			return MERROR::LISTEN_ERROR;
		}
		return 0;
	}

	int MAsyncServer::accept(const std::function<void(MAsyncServer&, NetAsyncIOAdapt)>& adapt_output)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		
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
			MLog::w("MAsyncServer::accept", "Bind Error Code is %d, Error Message is %s\n", MERROR::ACCEPT_ERROR, ec.message().c_str());
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

	MAsyncServer::iterator MAsyncServer::begin()
	{
		return this->m_data->sessions.begin();
	}

	MAsyncServer::iterator MAsyncServer::end()
	{
		return this->m_data->sessions.end();
	}

	NetAsyncIOAdapt& MAsyncServer::getNetAsyncIOAdapt(String UUID)
	{
		return this->m_data->sessions.at(UUID);
	}

	void MAsyncServer::earse(String UUID)
	{
		this->m_data->sessions.erase(UUID);
	}

	MAsyncServer::iterator MAsyncServer::earse(iterator& it)
	{
		return this->m_data->sessions.erase(it);
	}

}