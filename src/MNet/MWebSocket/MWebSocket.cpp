#include"MWebSocket.h"
#include<unordered_map>
namespace MUZI::net
{
	class MWebSocketConnectionData
	{
	public:
		MWebSocketConnectionData() :
			m_io_context_new_flag(true),
			m_io_context(new IOContext()),
			m_ws_ptr(new MWebSocketServer::MWebSocketConnection::WebSocketStream(boost::asio::make_strand(*this->m_io_context))),
			m_uuid()
		{
		}
		MWebSocketConnectionData(IOContext& io_context) :
			m_io_context_new_flag(false),
			m_io_context(&io_context),
			m_ws_ptr(new MWebSocketServer::MWebSocketConnection::WebSocketStream(boost::asio::make_strand(*this->m_io_context)))
		{
		}
		~MWebSocketConnectionData()
		{
			if (this->m_io_context_new_flag)
			{
				delete this->m_io_context;
				this->m_io_context = nullptr;
			}
		}
	public:
		std::unique_ptr<MWebSocketServer::MWebSocketConnection::WebSocketStream> m_ws_ptr;
		String m_uuid;
		IOContext* m_io_context;
		boost::beast::flat_buffer m_recv;
		std::queue<String> m_send_queue;
		std::mutex m_send_mutex;
		bool m_io_context_new_flag;
	};

	class MWebSocketServerData
	{
	public:
		MWebSocketServerData() :
			m_io_context_new_flag(true),
			m_io_context(new IOContext()),
			m_acceptor(*this->m_io_context)
		{
		}
		MWebSocketServerData(IOContext& io_context) :
			m_io_context_new_flag(false),
			m_io_context(&io_context),
			m_acceptor(*this->m_io_context)
		{}
		~MWebSocketServerData()
		{
			if (this->m_io_context_new_flag)
			{
				delete this->m_io_context;
				this->m_io_context = nullptr;
			}
		}
	public:
		bool m_io_context_new_flag;
		IOContext* m_io_context;
		TCPAcceptor m_acceptor;
		std::unordered_map<String, MWebSocketServer::MWebSocketConnection> m_conection_mapping;
	};

	String MWebSocketServer::MWebSocketConnection::createUUID()
	{
		static boost::uuids::random_generator rgen;
		return boost::uuids::to_string(rgen());
	}

	const String& MWebSocketServer::MWebSocketConnection::getUUID()
	{
		return this->m_data->m_uuid;
	}

	TCPSocket& MWebSocketServer::MWebSocketConnection::getSocket()
	{
		return boost::beast::get_lowest_layer(*(this->m_data->m_ws_ptr)).socket();
	}

	void MWebSocketServer::MWebSocketConnection::start()
	{
	}

	void MWebSocketServer::MWebSocketConnection::accept()
	{
		auto self = this->shared_from_this();
		this->m_data->m_ws_ptr->\
			async_accept(
				[self](const EC& ec)
				{
					if (!ec)
					{
						self->start();
					}
				});
	}

	MWebSocketServer::MWebSocketConnection::MWebSocketConnection() :
		m_data(new MWebSocketConnectionData())
	{
	}

	MWebSocketServer::MWebSocketConnection::MWebSocketConnection(IOContext& io_context) :
		m_data(new MWebSocketConnectionData(io_context))
	{
	}

	MWebSocketServer::MWebSocketServer() :
		m_data(new MWebSocketServerData())
	{
	}

	void MWebSocketServer::accept()
	{
		auto that = std::make_shared<MWebSocketConnection>(*this->m_data->m_io_context);
		this->m_data->m_acceptor.async_accept(
			boost::beast::get_lowest_layer(*that->m_data->m_ws_ptr).socket(),
			[this, that](const EC& ec)
			{
				if (!ec)
				{
					that->m_data->m_ws_ptr->\
						async_accept(
							[that](const EC& ec)
							{
								if (!ec)
								{
									that->accept();
									return;
								}
								MLog::w("MWebSocketServer::accept", "'async_accept' has error witch is %s", ec.what().c_str());
							});
					return;
				}
				MLog::w("MWebSocketServer::accept", "'async_accept' has error witch is %s", ec.what().c_str());
			});
	}

	void MWebSocketServer::read()
	{
	}
}