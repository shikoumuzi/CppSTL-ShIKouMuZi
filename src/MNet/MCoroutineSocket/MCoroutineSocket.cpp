#include"MCoroutineSocket.h"

namespace MUZI::net::coroutine
{
	class MCoroutineSocketData
	{
	public:
		MCoroutineSocketData(MCoroutineSocket::NotifiedFunction notified_function) :
			m_io_context(new IOContext()),
			m_new_io_context_flag(false),
			notified_fun(notified_function)
		{
		}
		MCoroutineSocketData(MCoroutineSocket::NotifiedFunction notified_function, IOContext& io_context) :
			m_io_context(&io_context),
			m_new_io_context_flag(true),
			notified_fun(notified_function)
		{
		}
		~MCoroutineSocketData()
		{
			if (this->m_new_io_context_flag)
			{
				delete this->m_io_context;
				this->m_io_context = nullptr;
			}
		}
	public:
		void handleAccept(MCoroSessionPack& session, const EC& error)
		{
			if (!error)
			{
				this->m_sessions[session->getUUID()] = session;
			}
		}
	public:
		IOContext* m_io_context;
		bool m_new_io_context_flag;
		std::map<std::string, MCoroSessionPack> m_sessions;  // 会话map

		MSyncAnnularQueue<MCoroSessionPack> session_notified_queue;  // 通知队列
		MCoroutineSocket::NotifiedFunction notified_fun;  // 通知函数

		std::atomic<bool> notified_thread_flag; // 通知逻辑层的原子锁
		std::thread notified_thread;  // 通知线程
		std::mutex notified_mutex;  // 通知锁
		std::condition_variable notified_cond;  // 通知条件变量
	};

	MCoroutineSocket::MCoroutineSocket(MCoroutineSocket::NotifiedFunction notified_function) :
		m_data(new MCoroutineSocketData(notified_function))
	{
	}
	MCoroutineSocket::MCoroutineSocket(MCoroutineSocket::NotifiedFunction notified_function, IOContext& io_context) :
		m_data(new MCoroutineSocketData(notified_function, io_context))
	{
	}

	IOContext& MCoroutineSocket::getIOContext()
	{
		return *this->m_data->m_io_context;
	}

	MCoroutineSocket::NotifiedLock MCoroutineSocket::getNotifiedLock()
	{
		return {
			.notified_mutex = this->m_data->notified_mutex,
			.notified_cond = this->m_data->notified_cond
		};
	}

	MCoroSessionPack& MCoroutineSocket::getSession(const String& uuid)
	{
		return this->m_data->m_sessions.at(uuid);
	}

	Map<String, MCoroSessionPack>& MCoroutineSocket::getSessions()
	{
		return this->m_data->m_sessions;
	}

	MCoroutineSocket::Awaitable<int> MCoroutineSocket::readFromSocket(MCoroSessionPack& session)
	{
		boost::asio::co_spawn(this->getIOContext(),
			[this, session]()->MCoroutineSocket::Awaitable<void>
			{
				auto session_tmp = session;
				session->recv_pending = true;
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

					while (session->recv_tmp_package->getCurSize() < session->recv_tmp_package->getTotalSize())
					{
						recv_byte = co_await \
							boost::asio::async_read(
								session->getSocket(),
								boost::asio::buffer(static_cast<char*>(session->recv_tmp_buff->getData()) + session->recv_tmp_buff->getCurSize(),
									header.total_size - session->recv_tmp_package->getCurSize()),
								boost::asio::use_awaitable);

						if (recv_byte == 0)
						{
							this->closeSession(session_tmp);
							co_return;
						}

						memcpy(static_cast<char*>(session->recv_tmp_package->getData()) + session->recv_tmp_package->getCurSize(),
							static_cast<char*>(session->recv_tmp_buff->getData()) + session->recv_tmp_package->getCurSize(), recv_byte);
						session->recv_tmp_package->getCurSize() += recv_byte;
					}

					static_cast<char*>(session->recv_tmp_package->getData())[session->recv_tmp_package->getTotalSize()] = '\0';
					session->recv_completed_queue.push(session->recv_tmp_package);

					session->recv_tmp_package->clear();
					session->recv_tmp_buff->clear();
					session->recv_pending = false;
					this->m_data->session_notified_queue.push(session_tmp);
					this->m_data->notified_cond.notify_all();

					co_return;
				}
			},
			boost::asio::detached);
		return Awaitable<int>();
	}

	MCoroutineSocket::Awaitable<int> MCoroutineSocket::writeToSocket, 0(MCoroSessionPack & session, const void* data, size_t data_size, int msg_id)
	{
		session->send_queue.push(std::make_shared<MSendMsgNode>(data, data_size, msg_id));
		if (session->isWriteCompleted())
		{
			return Awaitable<int>();
		}

		boost::asio::co_spawn(this->getIOContext(),
			[this, session]()->MCoroutineSocket::Awaitable<void>
			{
				while (!session->send_queue.empty())
				{
					auto session_tmp = session;
					session->send_tmp_buff = session->getPopFrontRecvMsg();

					auto send_byte = co_await \
						boost::asio::async_write(session->socket,
							boost::asio::buffer(session->send_tmp_buff->getData(), session->send_tmp_buff->getTotalSize()),
							boost::asio::use_awaitable);

					while (send_byte < session->send_tmp_buff->getTotalSize())
					{
						send_byte += co_await \
							boost::asio::async_write(session->socket,
								boost::asio::buffer(static_cast<char*>(session->send_tmp_buff->getData()) + send_byte, session->send_tmp_buff->getTotalSize() - send_byte),
								boost::asio::use_awaitable);
					}
				}
			},
			boost::asio::use_awaitable);

		return Awaitable<int>();
	}

	MCoroutineSocket::Awaitable<int> MCoroutineSocket::writeToSocket, 0(MCoroSessionPack & session, const std::string & data, int msg_id)
	{
		return this->writeToSocket, 0(session, data.data(), data.size(), msg_id);
	}

	void MCoroutineSocket::run()
	{
		this->m_data->m_io_context->run();
	}

	void MCoroutineSocket::closeSession(MCoroSessionPack& session)
	{
		session->socket.close();
		session->close_flag = true;
		this->erase(session->uuid);
	}

	MCoroutineSocket::iterator MCoroutineSocket::begin()
	{
		return this->m_data->m_sessions.begin();
	}

	MCoroutineSocket::iterator MCoroutineSocket::end()
	{
		return this->m_data->m_sessions.end();
	}

	void MCoroutineSocket::erase(String uuid)
	{
		this->m_data->m_sessions.erase(uuid);
	}

	MCoroutineSocket::iterator MCoroutineSocket::erase(iterator& it)
	{
		return this->m_data->m_sessions.erase(it);
	}
}