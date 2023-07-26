#include"MAsyncSocket.h"
#include<functional>

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

			auto& send_data = *send_data_ptr;
			send_data.getCurSize() += bytes_transaferred;
			if (send_data.getCurSize() < send_data.getTotalSize())
			{
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(send_data.getData()) + send_data.getCurSize(), send_data.getTotalSize() - send_data.getCurSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void {this->writeCallback(ec, adapt, size); });
				return;
			}
			// ��ʾͷԪ����ɴ���
			adapt->send_queue.pop();

			// �����Ϣ�������Ϊ�գ�����send_pending Ϊfalse ��������
			if (adapt->send_queue.empty())
			{
				adapt->send_pending = false;
			}
			else // �����Ϊ�����������
			{
				auto& o_send_data = **adapt->send_queue.front();
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
				send_data.getCurSize() += bytes_transaferred;
				adapt->socket.async_write_some(
					boost::asio::buffer(static_cast<char*>(send_data.getData()) + send_data.getCurSize(), send_data.getTotalSize() - send_data.getCurSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void {this->writeAllCallback(ec, adapt, size); });
				return;
			}
		}
		void readCallback(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				return;
			}
			
			auto& recv_data_ptr = *adapt->recv_queue.front();
			if (recv_data_ptr == nullptr)
			{
				return;
			}

			auto& recv_data = *recv_data_ptr;
			recv_data.getCurSize() += bytes_transaferred;
			if (recv_data.getCurSize() < recv_data.getTotalSize())
			{
				adapt->socket.async_read_some(
					boost::asio::buffer(static_cast<char*>(recv_data.getData()) + recv_data.getCurSize(), recv_data.getTotalSize() - recv_data.getCurSize()),
					[this, adapt](const EC& ec, std::size_t size) ->void 
					{
						this->readCallback(ec, adapt, size); 
					});
				//std::cout << static_cast<char*>(recv_data.getData()) << std::endl;

				return;
			}
			adapt->recv_pending = false;
			
		}
		void readAllCallback(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transaferred)
		{
			if (ec.value() != 0)
			{
				return;
			}
			/*adapt->recv_queue.pop();*/
			adapt->recv_pending = false;
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
			// ��ʾ�������ڷ���
			return 0;
		}
		// ��һ�ζ�����ƫ��
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
		// ˵����ǰ��Ȼ�ڶ�
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

	void MAsyncSocket::run()
	{
		this->m_data->io_context.run();
	}


}