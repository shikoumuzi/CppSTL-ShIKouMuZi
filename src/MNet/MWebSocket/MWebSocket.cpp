#include"MWebSocket.h"
#include<boost/beast.hpp>
#include<boost/beast/websocket.hpp>
#include<boost/beast/websocket/stream.hpp>

namespace MUZI::net
{
	class MWebSocketServerData
	{
	public:
		MWebSocketServerData() :
			m_io_context_new_flag(true),
			m_io_context(new IOContext()),
			m_ws_stream(boost::asio::make_strand(*this->m_io_context)),
			m_acceptor(*this->m_io_context)
		{
		}
		MWebSocketServerData(IOContext& io_context) :
			m_io_context_new_flag(false),
			m_io_context(&io_context),
			m_ws_stream(boost::asio::make_strand(*this->m_io_context)),
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
		boost::beast::websocket::stream<boost::beast::tcp_stream> m_ws_stream;
		bool m_io_context_new_flag;
		IOContext* m_io_context;
		TCPAcceptor m_acceptor;
	};

	MWebSocketServer::MWebSocketServer() :
		m_data(new MWebSocketServerData())
	{
	}
	void MWebSocketServer::accept()
	{
		this->m_data->m_acceptor.accept(boost::beast::get_lowest_layer(this->m_data->m_ws_stream).socket());
	}
}