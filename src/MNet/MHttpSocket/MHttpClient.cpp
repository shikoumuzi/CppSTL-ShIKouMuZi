#include"MHttpClient.h"
namespace MUZI::net::http
{
	MHttpClient::MHttpClient(const String& server, const String path) :
		m_io_context(new IOContext()),
		m_resolver(*this->m_io_context),
		m_socket(*this->m_io_context),
		m_new_io_context_flag(true)
	{
	}
	MHttpClient::MHttpClient(IOContext& io_context, const String& server, const String path) :
		m_resolver(io_context),
		m_io_context(&io_context),
		m_socket(io_context),
		m_new_io_context_flag(false)
	{
		std::ostream request_stream(&this->m_request);
		request_stream << "GET " << path << "HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connect: close\r\n\r\n";

		size_t pos = server.find(":");
		String ip = server.substr(0, pos);
		String port = server.substr(pos + 1);

		// 解析地址
		this->m_resolver.async_resolve(ip, port,
			[this](const EC& ec, const TCPResolver::results_type& endpoints)
			{
				this->handleResolver(ec, endpoints);
			});
	}
	MHttpClient::MHttpClient(MHttpClient&& client) :
		m_resolver(std::move(client.m_resolver)),
		m_socket(std::move(client.m_socket)),
		m_io_context(std::move(client.m_io_context))
	{
		client.m_io_context = nullptr;
	}
	MHttpClient::~MHttpClient()
	{
		if (this->m_new_io_context_flag)
		{
			delete this->m_io_context;
		}
		this->m_io_context = nullptr;
	}
	void MHttpClient::operator=(MHttpClient&& client)
	{
		this->m_socket.close();
		this->m_socket = std::move(client.m_socket);
		this->m_resolver.cancel();
		this->m_resolver = std::move(client.m_resolver);
		if (this->m_new_io_context_flag)
		{
			delete this->m_io_context;
		}
		this->m_io_context = std::move(client.m_io_context);
		client.m_io_context = nullptr;
	}
	void MHttpClient::handleResolver(const EC& ec, const TCPResolver::results_type& endpoints)
	{
		if (!ec)
		{
			// 连接服务器
			boost::asio::async_connect(
				this->getSocket(),
				endpoints,
				[this](const EC& ec, boost::asio::ip::tcp::endpoint endpoint)
				{
					this->handleConnect(ec, endpoint);
				});
		}
		MLog::w("MHttpClient::handleResolver", "error");
	}
	void MHttpClient::handleConnect(const EC& ec, boost::asio::ip::tcp::endpoint endpoint)
	{
		if (!ec)
		{
			// 发送request请求
			boost::asio::async_write(
				this->getSocket(),
				this->getRequest(),
				[this](const EC& ec, size_t byte)
				{
					this->handleWriteRequest(ec, byte);
				}
			);
		}
		MLog::w("MHttpClient::handleConnect", "error");
	}
	void MHttpClient::handleWriteRequest(const EC& ec, size_t byte)
	{
		if (!ec)
		{
			// 读取reponse
			boost::asio::async_read(
				this->getSocket(),
				this->getReponse(),
				[this](const EC& ec, size_t byte)
				{
					this->handleRead(ec, byte);
				}
			);
		}
		MLog::w("MHttpClient::handleWriteRequest", "error");
	}
	void MHttpClient::handleRead(const EC& ec, size_t byte)
	{
	}
	TCPSocket& MHttpClient::getSocket()
	{
		return this->m_socket;
	}
	Request& MHttpClient::getRequest()
	{
		return this->m_request;
	}
	Reponse& MHttpClient::getReponse()
	{
		return this->m_reponse;
	}
	IOContext& MHttpClient::getIOContext()
	{
		return *this->m_io_context;
	}
}