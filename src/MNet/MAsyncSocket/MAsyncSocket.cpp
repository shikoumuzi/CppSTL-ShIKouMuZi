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
			

			MRecvMsgNode& recv_buff = *adapt->recv_tmp_buff;
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

			// 如果已经完成复制，那么直接将结点数据拷贝到完成队列当中
			adapt->recv_completed_queue.push(adapt->recv_tmp_buff);

			adapt->recv_pending = false;
			adapt->recv_tmp_buff->clear();
			// 重新布置监听任务
			auto& o_recv_buff = *adapt->recv_tmp_buff;
			adapt->socket.async_read_some(
				boost::asio::buffer(static_cast<char*>(o_recv_buff.getData()), o_recv_buff.getTotalSize()),
				[this, adapt](const EC& ec, std::size_t size) ->void
				{
					this->readCallback(ec, adapt, size);
				});
			
			
		}
		void readAllCallback(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				return;
			}

			adapt->recv_completed_queue.push(adapt->recv_tmp_buff);
			adapt->recv_pending = false;
			adapt->recv_tmp_buff->clear();
			// 继续布置传输任务
			auto& recv_buff_ptr = adapt->recv_tmp_buff;
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

	int MAsyncSocket::wtiteToSocket(NetAsyncIOAdapt adapt, void* data, uint32_t size)
	{
		//  std::make_shared<MMsgNode>(new MMsgNode(data, size))
		adapt->send_queue.push(SendMsgPackage(new MSendMsgNode(data, size)));
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

	int MAsyncSocket::wtiteAllToSocket(NetAsyncIOAdapt adapt, void* data, uint32_t size)
	{
		adapt->send_queue.push(SendMsgPackage(new MSendMsgNode(data, size)));
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

	int MAsyncSocket::readFromSocket(NetAsyncIOAdapt adapt, uint32_t size)
	{
		// 说明当前仍然在读
		if (adapt->recv_pending){
			return 0;
		}

		adapt->socket.async_read_some(
			boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), size),
			[this, adapt](const EC& ec, std::size_t size) ->void
			{
				this->m_data->readCallback(ec, adapt, size); 
			});
		
		adapt->recv_pending = true;

		return 0;
	}

	int MAsyncSocket::readAllFromeSocket(NetAsyncIOAdapt adapt, uint32_t size)
	{
		if (adapt->recv_pending) {
			return 0;
		}

		adapt->socket.async_receive(boost::asio::buffer(adapt->recv_tmp_buff->getData(), size),
			[this, adapt](const EC& ec, std::size_t size) ->void 
			{ 
				this->m_data->readAllCallback(ec, adapt, size); 
			});

		return 0;
	}

	int MAsyncSocket::splitSendPackage(NetAsyncIOAdapt adapt, void* data, uint32_t size, uint32_t id)
	{
		uint32_t capacity = (size / __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__) + 1;
		uint32_t i = 0;
		for (; i < capacity - 1; ++i)
		{
			SendMsgPackage tmp_package(
				new MSendMsgNode(static_cast<char*>(data) + i * __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__));
			tmp_package->setId(id + 0);
			adapt->send_queue.push(tmp_package);
		}
		SendMsgPackage tmp_package(
			new MSendMsgNode(static_cast<char*>(data) + i * __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__));
		tmp_package->setId(id + 0);
		adapt->send_queue.push(tmp_package);

		return 0;
	}

	void MAsyncSocket::run()
	{
		this->m_data->io_context.run();
	}


}