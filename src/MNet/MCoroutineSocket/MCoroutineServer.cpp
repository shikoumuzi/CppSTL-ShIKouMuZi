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

	MCoroutineServer::MCoroutineServer(int& error_code, const MServerEndPoint& endpoint) :
		MCoroutineSocket(),
		m_data(new MCoroutineServerData(endpoint, this->getIOContext()))
	{
	}
	MCoroutineServer::MCoroutineServer(int& error_code, IOContext& context, const MServerEndPoint& endpoint) :
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
				[this, session, callback](const EC& ec) {
					auto session_tmp = session;
					auto callback_tmp = callback;
					callback(*this, session_tmp);
					this->accept(session_tmp, callback_tmp);
				});

		return 0;
	}

	void MCoroutineServer::startSession(MCoroSessionPack& session)
	{
		boost::asio::co_spawn(this->getIOContext(),
			[this, session]()->MCoroutineServer::Awaitable<void>
			{
				auto session_tmp = session;
				while (!session->isClose())
				{
					session->recv_tmp_buff->clear();
					size_t recv_byte = co_await\
						boost::asio::async_read(
							session->getSocket(),
							boost::asio::buffer(session->recv_tmp_buff->getData(), __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__),
							boost::asio::use_awaitable);
					if (recv_byte == 0)
					{
						this->closeSession(session_tmp);
						co_return;
					}


					memcpy(session->recv_tmp_package->getData(), session->recv_tmp_buff->getData(), recv_byte);
					session->recv_tmp_package->getCurSize() += recv_byte;

					auto header = session->recv_tmp_package->analyzeHeader();
					if (header.msg_id != __MUZI_MASYNCSOCKET_SPECIFICAL_PACKAGE_HEADER_ID__)
					{
						this->closeSession(session_tmp);
						co_return;
					}

					if (header.total_size > __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						this->closeSession(session_tmp);
						co_return;
					}

					recv_byte = co_await \
						boost::asio::async_read(
							session->getSocket(),
							boost::asio::buffer(static_cast<char*>(session->recv_tmp_buff->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
								header.msg_size),
							boost::asio::use_awaitable);

					session->recv_tmp_package->getCurSize() += recv_byte;


					if (recv_byte == 0)
					{
						this->closeSession(session_tmp);
						co_return;
					}

					memcpy(static_cast<char*>(session->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
						static_cast<char*>(session->recv_tmp_buff->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__, recv_byte);

					//if (session->recv_tmp_package->getCurSize() < session->recv_tmp_package->getTotalSize())
					//{
					//	recv_byte = co_await \
					//		boost::asio::async_read(
					//			session->getSocket(),
					//			boost::asio::buffer(static_cast<char*>(session->recv_tmp_buff->getData()) + session->recv_tmp_buff->getCurSize(),
					//				header.total_size - session->recv_tmp_package->getCurSize()),
					//			boost::asio::use_awaitable);
					//
					//}

					static_cast<char*>(session->recv_tmp_package->getData())[session->recv_tmp_package->getTotalSize()] = '\0';
					
					co_return;
				}
			},
			boost::asio::detached);
	}

	void MCoroutineServer::closeSession(MCoroSessionPack& session)
	{
		session->socket.close();
		session->close_flag = true;
		this->erase(session->uuid);
	}
}