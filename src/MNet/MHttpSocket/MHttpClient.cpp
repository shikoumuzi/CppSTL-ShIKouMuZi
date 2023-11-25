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
	MHttpClient::MHttpClient(MHttpClient&& client) noexcept:
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
	void MHttpClient::operator=(MHttpClient&& client) noexcept
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
				this->getResponse(),
				[this](const EC& ec, size_t byte)
				{
					this->handleReadStatusLine(ec, byte);
				}
			);
		}
		MLog::w("MHttpClient::handleWriteRequest", "error");
	}
	
	void MHttpClient::handleReadStatusLine(const EC& ec, size_t byte)
	{
		if (!ec)
		{
			//当收到对方数据时，先解析响应的头部信息
			std::istream response_stream(&this->m_response);
			String http_version;
			uint32_t status_code = 0;
			String status_msg;
			response_stream >> http_version;
			response_stream >> status_code;
			std::getline(response_stream, status_msg);
			// 读出HTTP版本，以及返回的状态码，如果状态码不是200，则返回，是200说明响应成功。
			if (!response_stream || http_version.substr(0, 5) != "HTTP/")
			{
				MLog::w("MHttpClient::handleReadStatusLine", "Invalid response");
				return;
			}
			if (status_code != 200)
			{
				MLog::w("MHttpClient::handleReadStatusLine", "Response returned with status code ");
				MLog::w("MHttpClient::handleReadStatusLine", "status code is %d\n", status_code);
				return;
			}

			boost::asio::async_read_until(this->getSocket(), this->getResponse(), "\r\n\r\n",
				[this](const EC& ec, size_t byte)
				{
					this->handleReadHeaders(ec, byte);
				});
		}
		else
		{
			MLog::w("MHttpClient::handleReadStatusLine", "Error is %s", ec.what().c_str());
		}
	}
	void MHttpClient::handleReadHeaders(const EC& ec, size_t byte)
	{
		if (!ec)
		{
			std::istream response_stream(&this->m_response);
			String header;
			while (std::getline(response_stream, header) && header != "r")
			{
				MLog::w("MHttpClient::handleReadHeaders", "header is %s\n", header.c_str());
			}

			if (this->m_response.size() > 0)
			{
				std::cout << &this->m_response;
			}

			boost::asio::async_read(
				this->m_socket,
				this->m_response,
				[this](const EC& ec, size_t byte)
				{
					this->handleReadContent(ec, byte);
				}
			);
		}
		else
		{
			MLog::w("MHttpClient::handleReadHeaders", "Error is %s", ec.what().c_str());
		}
		
	}
	void MHttpClient::handleReadContent(const EC& ec, size_t byte)
	{
		if (!ec)
		{
			// 读出响应的内容，继续监听读事件读取相应的内容，直到接收到EOF信息
			std::cout << &this->m_response << std::endl;
			boost::asio::async_read(this->m_socket, this->m_response,
				boost::asio::transfer_at_least(1),
				[this](const EC& ec, size_t byte)
				{
					this->handleReadContent(ec, byte);
				});
		}
		else
		{
			MLog::w("MHttpClient::handleReadContent", "Error is %s", ec.what().c_str());
		}
	}
	TCPSocket& MHttpClient::getSocket()
	{
		return this->m_socket;
	}
	RequestBuffer& MHttpClient::getRequest()
	{
		return this->m_request;
	}
	ReponseBuffer& MHttpClient::getResponse()
	{
		return this->m_response;
	}
	IOContext& MHttpClient::getIOContext()
	{
		return *this->m_io_context;
	}
	void MHttpClient::run()
	{
		this->m_io_context->run();
	}
}