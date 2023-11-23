#include"MCoroutineServer.h"

namespace MUZI::net::coroutine
{
	class MCoroutineServerData
	{
	public:
		MCoroutineServerData(const MServerEndPoint& endpoint, IOContext& io_context) :
			m_endpoint(endpoint), m_acceptor(io_context, *endpoint.getEndPoint())
		{
		}
		~MCoroutineServerData()
		{
		}

	public:
		MServerEndPoint m_endpoint;
		TCPAcceptor m_acceptor;
	};

	MCoroutineServer::MCoroutineServer(int& error_code, const MServerEndPoint& endpoint, NotifiedFunction notified_function) :
		MCoroutineSocket(notified_function),
		m_data(new MCoroutineServerData(endpoint, this->getIOContext()))
	{
	}
	MCoroutineServer::MCoroutineServer(int& error_code, IOContext& context, const MServerEndPoint& endpoint, NotifiedFunction notified_function) :
		MCoroutineSocket(notified_function, context),
		m_data(new MCoroutineServerData(endpoint, context))
	{
	}
	MCoroutineServer::~MCoroutineServer()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}
	int MCoroutineServer::listen(int back_log)
	{
		this->m_data->m_acceptor.listen(back_log);
		return 0;
	}

	int MCoroutineServer::accept(MCoroSessionPack& session, AcceptCallBack& callback)
	{
		this->m_data->m_acceptor.\
			async_accept(session->getSocket(),
				[this, session, callback](const EC& ec) {
					auto session_tmp = session;
					auto callback_tmp = callback;
					callback(*this, session_tmp);
					this->accept(session_tmp, callback_tmp);
				});

		return 0;
	}
}