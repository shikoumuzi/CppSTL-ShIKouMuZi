#include"MHttpClient.h"
namespace MUZI::net::http
{
	MHttpClient::MHttpClient(IOContext& io_context, const String& server, const String path) :
		m_resolver(io_context),
		m_io_context(&io_context),
		m_socket(io_context)
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
				this->handle_resolver(ec, endpoints);
			});
	}
	void MHttpClient::handle_resolver(const EC& ec, const TCPResolver::results_type& endpoints)
	{
		if (!ec)
		{
			// 连接服务器
			boost::asio::async_connect(
				this->getSocket(),
				endpoints,
				[this](const EC& ec, boost::asio::ip::tcp::endpoint endpoint)
				{
					this->handle_connect(ec, endpoint);
				});
		}
	}
	void MHttpClient::handle_connect(const EC& ec, boost::asio::ip::tcp::endpoint endpoint)
	{
		// 发送request请求
		boost::asio::async_write(
			this->getSocket(),
			this->getRequest(),
			[this](const EC& ec, size_t byte)
			{
				this->handle_write_request(ec, byte);
			}
		);
	}
	void MHttpClient::handle_write_request(const EC& ec, size_t byte)
	{
		if (!ec)
		{
			// 读取reponse
			boost::asio::async_read(
				this->getSocket(),
				this->getReponse(),
				[this](const EC& ec, size_t byte)
				{
					this->handle_read(ec, byte);
				}
			);
		}
	}
	void MHttpClient::handle_read(const EC& ec, size_t byte)
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
}