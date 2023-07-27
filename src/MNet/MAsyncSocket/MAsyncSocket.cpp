#include"MAsyncSocket.h"
#include<functional>
#include"MLog/MLog.h"
namespace MUZI::net::async
{

	class MAsyncSocket::MAsyncSocketData
	{
	public:
		MAsyncSocketData(MAsyncSocket* parent) :parent(parent)
		{}

	public:
		void writeCallback(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				return;
			}

			auto& send_data_ptr = *adapt->send_queue.front();
			if (send_data_ptr == nullptr)
			{
				return;
			}

			auto send_data = *send_data_ptr;
			send_data.getCurSize() += bytes_transaferred;
			if (send_data.getCurSize() < send_data.getTotalSize())
			{
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(send_data.getData()) + send_data.getCurSize(), send_data.getTotalSize() - send_data.getCurSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void {this->writeCallback(ec, adapt, size); });
				return;
			}
			// 表示头元素完成传输
			adapt->send_queue.pop();

			// 如果消息队列如果为空，就置send_pending 为false 结束传输
			if (adapt->send_queue.empty())
			{
				adapt->send_pending = false;
			}
			else // 如果不为空则继续发送
			{
				auto& o_send_data_ptr = *adapt->send_queue.front();
				if (send_data_ptr == nullptr)
				{
					return;
				}
				auto& o_send_data = *o_send_data_ptr;
				o_send_data.getCurSize() += bytes_transaferred;
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(o_send_data.getData()), o_send_data.getTotalSize()),
					[this, &adapt](const EC& ec, std::size_t size) ->void {this->writeCallback(ec, adapt, size); });
				return;
			}
		}
		void writeAllCallback(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				if (ec.value() == boost::asio::error::eof)
				{
					return;
				}
				return;
			}

			adapt->send_queue.pop();
			if (adapt->send_queue.empty())
			{
				adapt->send_pending = false;
			}
			else
			{
				auto& send_data_ptr = *adapt->send_queue.front();
				if (send_data_ptr == nullptr)
				{
					return;
				}

				auto& send_data = *send_data_ptr;
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(send_data.getData()), send_data.getTotalSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void {this->writeAllCallback(ec, adapt, size); });
				return;
			}
		}
		void readCallback(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncSocket::MAsyncSocketData::readCallback", "read is error, Error Message is: %s", ec.message().c_str());
				return;
			}
			
			auto& recv_buff_ptr = *adapt->recv_queue.front();
			if (recv_buff_ptr == nullptr)
			{
				return;
			}

			MMsgNode& recv_buff = *recv_buff_ptr;
			recv_buff.getCurSize() += bytes_transaferred;
			if (recv_buff.getCurSize() < recv_buff.getTotalSize())
			{
				adapt->socket.async_read_some(
					boost::asio::buffer(static_cast<char*>(recv_buff.getData()) + recv_buff.getCurSize(), recv_buff.getTotalSize() - recv_buff.getCurSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void 
					{
						this->readCallback(ec, adapt, size); 
					});

				return;
			}

			adapt->recv_completed_queue.push(*adapt->recv_queue.front());
			adapt->recv_queue.pop();

			if (adapt->recv_queue.empty())
			{
				adapt->recv_pending = false;
			}
			else
			{
				recv_buff_ptr = *adapt->recv_queue.front();
				if (recv_buff_ptr == nullptr)
				{
					return;
				}
				auto& o_recv_buff = *recv_buff_ptr;
				adapt->socket.async_read_some(
					boost::asio::buffer(static_cast<char*>(o_recv_buff.getData()), o_recv_buff.getTotalSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void
					{
						this->readCallback(ec, adapt, size);
					});
			}
			
		}
		void readAllCallback(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				return;
			}

			adapt->recv_completed_queue.push(*adapt->recv_queue.front());
			adapt->recv_queue.pop();
			if (adapt->recv_queue.empty())
			{
				adapt->recv_pending = false;
			}
			else
			{
				auto& recv_buff_ptr = *adapt->recv_queue.front();
				if (recv_buff_ptr == nullptr)
				{
					return;
				}
				auto& o_recv_buff = *recv_buff_ptr;
				adapt->socket.async_receive(
					boost::asio::buffer(o_recv_buff.getData(), o_recv_buff.getTotalSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void
					{
						this->readAllCallback(ec, adapt, size);
					});
			}
		}

		MAsyncSocket* parent;
		IOContext io_context;
	};



	MAsyncSocket::MAsyncSocket():m_data(new MAsyncSocketData(this))
	{}

	MAsyncSocket::~MAsyncSocket()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
		}
	}

	IOContext& MAsyncSocket::getIOContext()
	{
		return this->m_data->io_context;
	}


	int MAsyncSocket::writeToSocket(NetAsyncIOAdapt adapt, String& data)
	{
		return this->wtiteToSocket(adapt, (void*)(data.c_str()), data.size());
	}

	int MAsyncSocket::wtiteToSocket(NetAsyncIOAdapt adapt, void* data, uint64_t size)
	{
		//  std::make_shared<MMsgNode>(new MMsgNode(data, size))
		adapt->send_queue.push(MsgPackage(new MMsgNode(data, size)));
		if (adapt->send_pending) {
			// 表示现在正在发送
			return 0;
		}
		// 第一次读不用偏移
		adapt->socket.async_write_some(
			boost::asio::buffer(static_cast<char*>(data), size), 
			[this, adapt](const EC& ec, std::size_t size) ->void 
			{
				this->m_data->writeCallback(ec, adapt, size); 
			});

		adapt->send_pending = true;
		return 0;
	}

	int MAsyncSocket::wtiteAllToSocket(NetAsyncIOAdapt adapt, void* data, uint64_t size)
	{
		adapt->send_queue.push(MsgPackage(new MMsgNode(data, size)));
		if (adapt->send_pending) {
			return 0;
		}
		adapt->socket.async_send(
			boost::asio::buffer(data, size),
			[this, adapt](const EC& ec, std::size_t size)->void 
			{
				this->m_data->writeAllCallback(ec, adapt, size);
			});
		return 0;
	}

	int MAsyncSocket::readFromSocket(NetAsyncIOAdapt adapt, uint64_t size)
	{
		adapt->recv_queue.push(MsgPackage(new MMsgNode(nullptr, size, true)));
		// 说明当前仍然在读
		if (adapt->recv_pending){
			return 0;
		}
		
		auto& recv_buff = *adapt->recv_queue.front();
		if (recv_buff == nullptr)
		{
			return 0;
		}

		adapt->socket.async_read_some(
			boost::asio::buffer(static_cast<char*>(recv_buff->getData()), size),
			[this, adapt](const EC& ec, std::size_t size) ->void
			{
				this->m_data->readCallback(ec, adapt, size); 
			});
		
		adapt->recv_pending = true;

		return 0;
	}

	int MAsyncSocket::readAllFromeSocket(NetAsyncIOAdapt adapt, uint64_t size)
	{
		adapt->recv_queue.push(MsgPackage(new MMsgNode(nullptr, size, true)));
		if (adapt->recv_pending) {
			return 0;
		}
		auto& recv_buff = *adapt->recv_queue.front();
		if (recv_buff == nullptr)
		{
			return 0;
		}

		adapt->socket.async_receive(boost::asio::buffer(recv_buff->getData(), size),
			[this, adapt](const EC& ec, std::size_t size) ->void 
			{ 
				this->m_data->readAllCallback(ec, adapt, size); 
			});

		return 0;
	}

	int MAsyncSocket::splitSendPackage(NetAsyncIOAdapt adapt, void* data, uint64_t size)
	{
		uint64_t capacity = (size / __MUZI_MASYNCSOCKET_PACKAGE_SIZE_IN_BYTES__) + 1;
		uint64_t i = 0;
		for (; i < capacity - 1; ++i)
		{
			MsgPackage tmp_package(
				new MMsgNode(static_cast<char*>(data) + i * __MUZI_MASYNCSOCKET_PACKAGE_SIZE_IN_BYTES__, __MUZI_MASYNCSOCKET_PACKAGE_SIZE_IN_BYTES__));
			tmp_package->setId(i + 1);
			adapt->send_queue.push(tmp_package);
		}
		MsgPackage tmp_package(
			new MMsgNode(static_cast<char*>(data) + i * __MUZI_MASYNCSOCKET_PACKAGE_SIZE_IN_BYTES__, __MUZI_MASYNCSOCKET_PACKAGE_SIZE_IN_BYTES__));
		tmp_package->setId(i + 1);
		adapt->send_queue.push(tmp_package);

		return 0;
	}

	void MAsyncSocket::run()
	{
		this->m_data->io_context.run();
	}


}