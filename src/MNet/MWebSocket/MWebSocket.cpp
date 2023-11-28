#include"MWebSocket.h"
#include<unordered_map>
namespace MUZI::net
{
	class MWebSocketConnectionData
	{
	public:
		MWebSocketConnectionData(MWebSocketServer* parent) :
			parent(parent),
			m_io_context_new_flag(true),
			m_io_context(new IOContext()),
			m_ws_ptr(new MWebSocketServer::MWebSocketConnection::WebSocketStream(boost::asio::make_strand(*this->m_io_context))),
			m_uuid()
		{
		}
		MWebSocketConnectionData(MWebSocketServer* parent,
			IOContext& io_context) :
			parent(parent),
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
		boost::beast::flat_buffer m_recv_buffer;
		std::queue<String> m_send_queue;
		std::mutex m_send_mutex;
		bool m_io_context_new_flag;
		MWebSocketServer* parent;
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
		std::unordered_map<String, std::shared_ptr<MWebSocketServer::MWebSocketConnection>> m_conection_mapping;
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
		auto self = this->shared_from_this();
		this->m_data->m_ws_ptr->async_read(
			this->m_data->m_recv_buffer,
			[self](const EC& ec, size_t byte_transafered)
			{
				if (!ec)
				{
					// 在这进行buffer_to_string 处理

					// 继续调用使得数据继续解读，维持websocket 流
					self->start();
					return;
				}
				self->m_data->parent->m_data->m_conection_mapping.erase(self->m_data->m_uuid);
				MLog::w("MWebSocketServer::MWebSocketConnection::start", "'async_read' has error witch is %s", ec.what().c_str());
			});
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
						self->m_data->parent->m_data->m_conection_mapping[self->getUUID()] = self;
						self->start();
					}
					MLog::w("MWebSocketServer::MWebSocketConnection::accept", "'async_accept' has error witch is %s", ec.what().c_str());
				});
	}

	void MWebSocketServer::MWebSocketConnection::send(const void* data, size_t data_size)
	{
		this->send(String(static_cast<const char*>(data), data_size));
	}

	void MWebSocketServer::MWebSocketConnection::send(const String& data)
	{
		{
			std::lock_guard<std::mutex> g_lock(this->m_data->m_send_mutex);
			size_t que_len = this->m_data->m_send_queue.size();
			this->m_data->m_send_queue.push(data);
			if (que_len > 0)
			{
				return;
			}
		}

		auto self = this->shared_from_this();
		this->m_data->m_ws_ptr->async_write(
			boost::asio::buffer(data.c_str(), data.size()),
			[self](const EC& ec, size_t byte_transafered)
			{
				if (ec)
				{
					MLog::w("MWebSocketServer::MWebSocketConnection::send", "'async_write' has error witch is %s", ec.what().c_str());
					return;
				}
				String send_msg;
				{
					std::lock_guard<std::mutex> glock(self->m_data->m_send_mutex);
					if (self->m_data->m_send_queue.empty())
					{
						return;
					}
					send_msg = std::move(self->m_data->m_send_queue.front());
					self->m_data->m_send_queue.pop();
				}
				self->send(std::move(send_msg));
			}
		);
	}

	MWebSocketServer::MWebSocketConnection::MWebSocketConnection(MWebSocketServer* parent) :
		m_data(new MWebSocketConnectionData(parent))
	{
	}

	MWebSocketServer::MWebSocketConnection::MWebSocketConnection(MWebSocketServer* parent, IOContext& io_context) :
		m_data(new MWebSocketConnectionData(parent, io_context))
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
					that->accept();
					return;
				}
				MLog::w("MWebSocketServer::accept", "'async_accept' has error witch is %s", ec.what().c_str());
			});
	}

	void MWebSocketServer::addConnection()
	{
	}
	void MWebSocketServer::eraseConnection()
	{
	}
}