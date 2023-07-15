#include"MAsyncServerSocket.h"
#include<functional>

namespace MUZI::NET::ASYNC
{

	class MAsyncServerSocket::MAsyncServerSocketData
	{
	public:
		MAsyncServerSocketData(MAsyncServerSocket* parent, MServerEndPoint& endpoint):parent(parent), io_context(IOContext()), acceptor(io_context)
		{
			int error_code;
			EC ec;
			this->acceptor.bind(*endpoint.getEndPoint(error_code), ec);
			
		}

	public:
		void writeCallback(const EC& ec, NetAsyncIOAdapt& adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				return;
			}
			auto& send_data = *adapt->msg_queue.front();
			send_data.cur_size += bytes_transaferred;
			if (send_data.cur_size < send_data.total_size)
			{
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(send_data.data) + send_data.cur_size, send_data.total_size - send_data.cur_size),
					[this, &adapt](const EC& ec, std::size_t size) ->void {this->writeCallback(ec, adapt, size); });
				return;
			}
			// 表示头元素完成传输
			adapt->msg_queue.pop();

			// 如果消息队列如果为空，就置send_pending 为false 结束传输
			if (adapt->msg_queue.empty())
			{
				adapt->send_pending = false;
			}
			else // 如果不为空则继续发送
			{
				send_data = *adapt->msg_queue.front();
				send_data.cur_size += bytes_transaferred;
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(send_data.data) + send_data.cur_size, send_data.total_size - send_data.cur_size),
					[this, &adapt](const EC& ec, std::size_t size) ->void {this->writeCallback(ec, adapt, size); });
				return;
			}
		}
		void writeAllCallback(const EC& ec, NetAsyncIOAdapt& adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				return;
			}

			adapt->msg_queue.pop();
			if (adapt->msg_queue.empty())
			{
				adapt->send_pending = false;
			}
			else
			{
				auto& send_data = *adapt->msg_queue.front();
				send_data.cur_size += bytes_transaferred;
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(send_data.data) + send_data.cur_size, send_data.total_size - send_data.cur_size),
					[this, &adapt](const EC& ec, std::size_t size) ->void {this->writeAllCallback(ec, adapt, size); });
				return;
			}
		}

		MAsyncServerSocket* parent;
		IOContext io_context;
		TCPAcceptor acceptor;
	};



	MAsyncServerSocket::MAsyncServerSocket(MServerEndPoint& endpoint):m_data(new MAsyncServerSocketData(this, endpoint))
	{
		
	}

	MAsyncServerSocket::~MAsyncServerSocket()
	{
	}

	int MAsyncServerSocket::listen(int back_log)
	{
		this->m_data->acceptor.listen(back_log);
		return 0;
	}

	NetAsyncIOAdapt MAsyncServerSocket::accept(int& ec)
	{

		return NetAsyncIOAdapt();
	}

	int MAsyncServerSocket::writeToSocket(NetAsyncIOAdapt& adapt, String& data)
	{
		return this->wtiteToSocket(adapt, (void*)(data.c_str()), data.size());
	}

	int MAsyncServerSocket::wtiteToSocket(NetAsyncIOAdapt& adapt, void* data, uint64_t size)
	{
		adapt->msg_queue.emplace(new MsgNode(data, size));
		if (adapt->send_pending) {
			// 表示现在正在发送
			return 0;
		}
		// 第一次读不用偏移
		adapt->socket.async_write_some(
			boost::asio::buffer(static_cast<char*>(data), size), 
			[this, &adapt](const EC& ec, std::size_t size) ->void {this->m_data->writeCallback(ec, adapt, size); });

		adapt->send_pending = true;
		return 0;
	}

	int MAsyncServerSocket::wtiteAllToSocket(NetAsyncIOAdapt& adapt, void* data, uint64_t size)
	{
		adapt->msg_queue.emplace(new MsgNode(data, size));
		if (adapt->send_pending) {
			return;
		}
		adapt->socket.async_send(
			boost::asio::buffer(data, size),
			[this, &adapt](const EC& ec, std::size_t size)->void {this->m_data->writeAllCallback(ec, adapt, size); });
		return 0;
	}

	Session::Session(TCPSocket socket) : socket(std::move(socket)), send_pending(false), recv_pending(false);
	{
	}

	Session::~Session()
	{
	}

}