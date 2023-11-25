#include"MHttpServer.h"
namespace MUZI::net::http
{
	class HttpConnectionData
	{
	public:
		HttpConnectionData(TCPSocket& socket, size_t buffer_size = __MUZI_MHTTPSERVER_BUFFER_SIZE__, size_t time_out = __MUZI_MHTTPSERVER_TIMEOUT_VALUE__) :
			m_socket(std::move(socket)),
			m_buffer(buffer_size),
			m_timer(this->m_socket.get_executor(), std::chrono::seconds(time_out)) // 设置超时时间，如果超过该值还没处理完http则返回
		{}
	public:
		TCPSocket m_socket;
		boost::beast::flat_buffer m_buffer;
		boost::beast::http::request<boost::beast::http::dynamic_body> m_request; // 动态包体
		boost::beast::http::response<boost::beast::http::dynamic_body> m_reponse;
		boost::beast::net::steady_timer m_timer;
	};

	class HttpServerData
	{
	public:

	};

	MHttpServer::MHttpServer()
	{
	}
	MHttpServer::HttpConnection::HttpConnection(
		TCPSocket& socket, 
		size_t buffer_size = __MUZI_MHTTPSERVER_BUFFER_SIZE__, 
		size_t time_out = __MUZI_MHTTPSERVER_TIMEOUT_VALUE__):
		m_data(new HttpConnectionData(socket, buffer_size, time_out))
	{
	}
	MHttpServer::HttpConnection::~HttpConnection()
	{
	}
	void MHttpServer::HttpConnection::start()
	{
	}
	void MHttpServer::HttpConnection::readRequest()
	{
		auto self = shared_from_this();
	}
	void MHttpServer::HttpConnection::checkDeadline()
	{
		auto self = shared_from_this();
		// 该智能指针是为了保证当超时时，this对象不为空，相当于采用闭包延长connection生命周期
		this->m_data->m_timer.async_wait(
			[self](const EC& ec)
			{
				if (!ec)
				{
					self->m_data->m_socket.close();
				}
			});
	}
}