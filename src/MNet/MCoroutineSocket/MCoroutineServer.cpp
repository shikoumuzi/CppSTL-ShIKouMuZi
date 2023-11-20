#include"MCoroutineServer.h"

namespace MUZI::net::coroutine
{
	class MCoroutineServerData
	{
	public:
		MCoroutineServerData(const MServerEndPoint& endpoint, IOContext& io_context):
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


	MCoroutineServer::MCoroutineServer(int& error_code, const MServerEndPoint& endpoint):
		MCoroutineSocket(), 
		m_data(new MCoroutineServerData(endpoint, this->getIOContext()))
	{

	}
	MCoroutineServer::MCoroutineServer(int& error_code, IOContext& context, const MServerEndPoint& endpoint):
		MCoroutineSocket(context), 
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
				[this, &session, &callback](const EC& ec) {
					callback(*this, session);
					this->accept(session, callback);
				});

		return 0;
	}

	void MCoroutineServer::startSession(MCoroSessionPack& session)
	{
		boost::asio::co_spawn(this->getIOContext(),
			[this, session]()->MCoroutineServer::Awaitable<void>
			{
				while (!session->isClose())
				{
					
				}
			},
			boost::asio::detached);
	}

	void MCoroutineServer::closeSession(MCoroSessionPack& session)
	{
	}




}