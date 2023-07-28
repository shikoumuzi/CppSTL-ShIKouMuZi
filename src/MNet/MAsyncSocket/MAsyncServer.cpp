#include "MAsyncServer.h"

namespace MUZI::net::async
{
	class MAsyncServer::MAsyncServerData
	{
	public:
		MAsyncServerData(MAsyncServer* parent, IOContext& io_context, const MServerEndPoint& endpoint)
			: parent(parent), acceptor(io_context, *endpoint.getEndPoint())
		{}
	public:
		int accpetCallback(NetAsyncIOAdapt adapt, const EC& ec)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::handle_accpet", "async_handle error, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message());
				return MERROR::ACCEPT_ERROR;
			}
			// 连接成功了才放入池子当中
			this->sessions.emplace(adapt->getUUID(), adapt);
			//this->parent->readFromSocket(adapt);
			return 0;
		}

	public:
		void handleRread(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transafered)
		{
			if (ec.value() != 0)
			{
				return;
			}

			int copy_len = 0;
			MsgPackage recv_data_ptr = nullptr;
			while (bytes_transafered > 0)
			{
				recv_data_ptr = adapt->recv_tmp_buff;

				// 头部没有接收完成
				if (!adapt->head_parse)
				{
					// 头部数据没有接收完全,即头部数据小于头部规定长度
					if (bytes_transafered + recv_data_ptr->getCurSize() < __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{







						return;
					}
					// 如果收到的数据比头部多,但头部又没处理完（!recv_data_ptr->getHeadParse()）
					// 头部剩余没有复制的长度
					int head_remain = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__ - recv_data_ptr->getCurSize();

					// 将头部剩余数据先缓存到recv_tmp_package当中
					memcpy(adapt->recv_tmp_package->getData(), recv_data_ptr->getData(), head_remain);

					// 更新已处理的data长度和剩余未处理的长度
					copy_len += head_remain;
					bytes_transafered -= head_remain;

					// 获取头部数据
					MMsgNode::MMsgNodeDataBaseMsg& header = adapt->recv_tmp_package->analyzeHeader();
					adapt->recv_tmp_package->getCurSize() += head_remain;
					// 表示头部长度非法
					if (header.total_size > __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						// 删除链接
						this->parent->earse(adapt->getUUID());
						return;
					}

					// 如果接收到的数据小于总长度，那么就先把这些从缓存中取出来然后接着接收
					if (bytes_transafered < header.total_size)
					{
						// 先取出缓存
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEAD_SIZE_IN_BYTES__,
							recv_data_ptr->getData(), bytes_transafered);
						// 保留头部，重新部署监听任务
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(recv_data_ptr->getData()) + __MUZI_MMSGNODE_MSGNODE_HEAD_SIZE_IN_BYTES__,
												__MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, std::size_t size)->void
							{
								this->handleRread(ec, adapt, size);
							});
						adapt->head_parse = true;
						return;
					}

					// 如果接收到的数据小于总长度，发生了粘包现像, 此时包中假定存在完整数据
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEAD_SIZE_IN_BYTES__,
							recv_data_ptr->getData(), header.msg_size);
					adapt->recv_tmp_package->getCurSize() += header.msg_size + 1;
					copy_len += header.msg_size;
					bytes_transafered -= header.msg_size;
					// 添加终止符
					static_cast<char*>(adapt->recv_tmp_package->getData())[(adapt->recv_tmp_package->getTotalSize() - 1)] = '\0';

					adapt->head_parse = false;
					adapt->recv_tmp_buff->clear();
					if (bytes_transafered <= 0)
					{
						return;
					}

					continue;
				}



			}
		}
	public:
		MAsyncServer* parent;
		TCPAcceptor acceptor;
		std::map<std::string, NetAsyncIOAdapt> sessions;
	};


	MAsyncServer::MAsyncServer(int& error_code, const MServerEndPoint& endpoint) 
		:m_data(new MAsyncServerData(this, this->getIOContext(), endpoint))
	{}

	MAsyncServer::~MAsyncServer()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	int MAsyncServer::listen(int back_log)
	{
		EC ec;
		this->m_data->acceptor.listen(back_log, ec);
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer::listen", "listen is error, Error Code is %d, Error Messahe is %s\n", MERROR::LISTEN_ERROR, ec.message().c_str());
			return MERROR::LISTEN_ERROR;
		}
		return 0;
	}

	int MAsyncServer::accept(const std::function<void(MAsyncServer&, NetAsyncIOAdapt)>& adapt_output)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		
		this->m_data->acceptor.async_accept(adapt->socket, 
			[this, adapt, adapt_output](const EC& ec)->void
			{
				if (this->m_data->accpetCallback(adapt, ec) == 0) {
					adapt_output(*this, adapt);
					this->accept(adapt_output);
				}
			});
		if (ec.value() != 0)
		{
			MLog::w("MAsyncServer::accept", "Bind Error Code is %d, Error Message is %s\n", MERROR::ACCEPT_ERROR, ec.message().c_str());
			return MERROR::ACCEPT_ERROR;
		}
		return 0;
	}

	NetAsyncIOAdapt MAsyncServer::accept(int& error_code)
	{
		EC ec;
		NetAsyncIOAdapt adapt(new MSession(TCPSocket(this->getIOContext())));
		/*std::make_shared<TCPSocket>(std::move(TCPSocket(this->m_data->io_context, this->m_data->protocol)));*/
		this->m_data->acceptor.accept(adapt->getSocket(), ec);
		if (ec.value() != 0)
		{
			std::cerr << ec.message();
			/*MLog::w("MSyncSocket::accept", "accept is failed, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message().c_str());*/
			error_code = MERROR::ACCEPT_ERROR;
			return NetAsyncIOAdapt();
		}
		return adapt;
	}

	MAsyncServer::iterator MAsyncServer::begin()
	{
		return this->m_data->sessions.begin();
	}

	MAsyncServer::iterator MAsyncServer::end()
	{
		return this->m_data->sessions.end();
	}

	NetAsyncIOAdapt& MAsyncServer::getNetAsyncIOAdapt(String UUID)
	{
		return this->m_data->sessions.at(UUID);
	}

	void MAsyncServer::earse(String UUID)
	{
		this->m_data->sessions.erase(UUID);
	}

	MAsyncServer::iterator MAsyncServer::earse(iterator& it)
	{
		return this->m_data->sessions.erase(it);
	}

}