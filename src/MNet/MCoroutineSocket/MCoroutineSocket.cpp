#include"MCoroutineSocket.h"

namespace MUZI::net::coroutine
{
	class MCoroutineSocketData
	{
	public:
		MCoroutineSocketData():
			m_io_context(new IOContext()),
			m_new_io_context_flag(false)
		{

		}
		MCoroutineSocketData(IOContext& io_context):
			m_io_context(&io_context),
			m_new_io_context_flag(true)
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
		void handleAccept(MCoroSessionPack& session, const EC &error)
		{
			if (!error)
			{
				this->m_sessions[session->getUUID()] = session;
			}
		}
	public:
		IOContext* m_io_context;
		bool m_new_io_context_flag;
		std::map<std::string, MCoroSessionPack> m_sessions;  // »á»°map
	};

	MCoroutineSocket::MCoroutineSocket():
		m_data(new MCoroutineSocketData())
	{
	}
	MCoroutineSocket::MCoroutineSocket(IOContext& io_context):
		m_data(new MCoroutineSocketData(io_context))
	{
	}

	IOContext& MCoroutineSocket::getIOContext()
	{
		return *this->m_data->m_io_context;
	}

	MCoroSessionPack& MCoroutineSocket::getSession(const String& uuid)
	{
		return this->m_data->m_sessions.at(uuid);
	}

	Map<String, MCoroSessionPack>& MCoroutineSocket::getSessions()
	{
		return this->m_data->m_sessions;
	}


	void MCoroutineSocket::run()
	{
		this->m_data->m_io_context->run();
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
		this->m_data->m_sessions.erase(it);
	}

}


