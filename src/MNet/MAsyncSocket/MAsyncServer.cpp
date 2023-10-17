#include "MAsyncServer.h"
#include<thread>
#include<mutex>
#include<atomic>

namespace MUZI::net::async
{


	class MAsyncServer::MAsyncServerData
	{
	public:
		using AnalyzedHeader = MMsgNodeDataBaseMsg(*)(MRecvMsgNode&);

	public:
		MAsyncServerData(MAsyncServer* parent, IOContext& io_context, const MServerEndPoint& endpoint, NotifiedFunction notified_fun)
			: parent(parent), acceptor(io_context, *endpoint.getEndPoint()),
			notified_fun(notified_fun),
			notified_thread_flag(true)
		{
			this->notified_thread =
				std::move(std::thread(
					[this]()
					{
						bool notified_pending = false;
						std::unique_lock<std::mutex> notified_lock(this->notified_mutex);
						NetAsyncIOAdapt* adapt;
						while (this->notified_thread_flag)
						{
							this->notified_cond.wait(notified_lock);

							// 队列是否为空 和 是否仍然有处于通知中的内容
							if (!this->session_notified_queue.empty() && !notified_pending)
							{
								notified_pending = true;
								this->notified_fun(*this->parent);
								notified_pending = false;
							}
							
						}
					}));
			this->notified_thread.detach();
		}
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
		void handleRawHeaderRread(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transafered)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::MAsyncServerData::handleRread", "Error Code is %d, Error Message is %s", MERROR::RECV_ERROR, ec.message().c_str());
				this->parent->earse(adapt->getUUID());
				return;
			}

			int copy_len = 0;
			while (bytes_transafered > 0)
			{
				adapt->recv_tmp_buff = adapt->recv_tmp_buff;

				// 头部没有接收完成
				if (!adapt->head_parse)
				{
					// 头部数据没有接收完全,即数据小于头部规定长度， 先复制一部分，然后继续监听
					if (bytes_transafered + adapt->recv_tmp_buff->getCurSize() < __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();
						
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRawHeaderRread(ec, adapt, size);
							});

						return;
					}
					// 如果收到的数据比头部多,但头部又没处理完（!adapt->recv_tmp_buff->getHeadParse()）
					// 头部剩余没有复制的长度
					int head_remain = __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__ - adapt->recv_tmp_buff->getCurSize();

					// 将头部剩余数据先缓存到recv_tmp_package当中
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// 更新已处理的data长度和剩余未处理的长度
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;

					// 获取头部数据,并更新缓存包
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeRawHeader();
					adapt->recv_tmp_package->getCurSize() += head_remain;
					if (header.msg_id > __MUZI_MMSGNODE_MSGNODE_DEFAULT_MAX_MAG_ID__)
					{
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// 表示头部长度非法
					if (header.total_size > __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						// 删除链接
						this->parent->earse(adapt->getUUID());
						return;
					}

					// 如果接收到的数据小于总长度，说明数据没有收全，那么就先把这些从缓存中取出来然后接着接收
					if (bytes_transafered < header.total_size)
					{
						// 先取出缓存
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// 更新
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// 重新部署监听任务, 此时数据结构不需要再解析头部，由adapt->head_parse记录已解析，recv_tmp_package记录之前接收的头部数据
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRawHeaderRread(ec, adapt, size);
							});
						adapt->head_parse = true;


						// 交由async_read_some，继续接收剩余数据
						return;
					}

					// 如果接收到的数据大于总长度，发生了粘包现像, 此时包中假定存在完整数据
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), header.msg_size);

					adapt->recv_tmp_package->getCurSize() += header.msg_size + 1;
					copy_len += header.msg_size;
					bytes_transafered -= header.msg_size;

					// 添加终止符
					static_cast<char*>(adapt->recv_tmp_package->getData())[(adapt->recv_tmp_package->getTotalSize() - 1)] = '\0';

					//将接收结果推送到队列当中
					adapt->recv_completed_queue.push(adapt->recv_tmp_package);

					// 通知接收到一个包
					this->session_notified_queue.push(adapt);
					this->notified_cond.notify_all();

					// 获取结束，重新初始化内容
					adapt->head_parse = false;
					// 重新构造结点
					adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();
					// 清空接收缓冲区
					adapt->recv_tmp_buff->clear();

					// 如果读完，则重新布置监听任务
					if (bytes_transafered <= 0)
					{
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRawHeaderRread(ec, adapt, size);
							});
						return;
					}

					// 未读完则继续解析内容
					continue;
				}

				//已经处理完头部，处理上次未接受完的消息数据
				//接收的数据仍不足剩余未处理的
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeRawHeader();
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// 如果说接受的数据还是少于所需求的
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// 重新布置监听任务
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleRawHeaderRread(ec, adapt, size);
						});
					return;
				}

				// 如果接受数据长度大于等于所需要的信息长度，则代表发生了粘包现象
				memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
					adapt->recv_tmp_buff->getData(), bytes_transafered);

				// 更新接收数据
				adapt->recv_tmp_package->getCurSize() += msg_remain + 1;
				bytes_transafered -= msg_remain;
				copy_len += msg_remain;

				// 添加终止符
				static_cast<char*>(adapt->recv_tmp_package->getData())[adapt->recv_tmp_package->getCurSize() + 1] = '\0';

				// 加入完成队列
				adapt->recv_completed_queue.push(adapt->recv_tmp_package);

				// 通知接收到一个包，并将session传给通知队列
				this->session_notified_queue.push(adapt);
				this->notified_cond.notify_all();

				// 重新构造节点
				adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();

				adapt->head_parse = false;
				adapt->recv_tmp_buff->clear();

				if (bytes_transafered <= 0)
				{
					// 重新布置监听任务
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
						[this, adapt](const EC& ec, size_t size)->void
						{
							this->handleRawHeaderRread(ec, adapt, size);
						});
					return;
				}

				continue;
			}
		}
		void handleJsonHeaderRread(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transafered)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::MAsyncServerData::handleRread", "Error Code is %d, Error Message is %s", MERROR::RECV_ERROR, ec.message().c_str());
				this->parent->earse(adapt->getUUID());
				return;
			}

			int copy_len = 0;
			while (bytes_transafered > 0)
			{
				adapt->recv_tmp_buff = adapt->recv_tmp_buff;

				// 头部没有接收完成
				if (!adapt->head_parse)
				{
					// 头部数据没有接收完全,即数据小于头部规定长度， 先复制一部分，然后继续监听
					if (bytes_transafered + adapt->recv_tmp_buff->getCurSize() < __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_LEAST__)
					{
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleJsonHeaderRread(ec, adapt, size);
							});

						return;
					}

					// 如果收到的数据比头部多,但头部又没处理完（!adapt->recv_tmp_buff->getHeadParse()）
					// 头部可能剩余没有复制的长度
					int head_remain = __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_LEAST__ - adapt->recv_tmp_buff->getCurSize();

					// 将头部剩余数据先缓存到recv_tmp_package当中
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// 更新已处理的data长度和剩余未处理的长度
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;
					adapt->recv_tmp_package->getCurSize() += head_remain;

					uint32_t json_header_string_size = 0; // 头部json长度 
					// 获取头部数据,并更新缓存包
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeJsonHeader(json_header_string_size);
					// 表示数量接近够了但是解析失败，尝试再接收一次
					if (header.msg_id == -1)
					{
						// 重新部署监听任务, 此时数据结构不需要再解析头部，由adapt->head_parse记录已解析，recv_tmp_package记录之前接收的头部数据
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), 
								__MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__ - adapt->recv_tmp_package->getCurSize()),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleJsonHeaderRread(ec, adapt, size);
							});
						return;
					}

					// 表示头部长度非法
					if (header.total_size > __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__)
					{
						// 删除链接
						this->parent->earse(adapt->getUUID());
						return;
					}

					// 如果接收到的数据小于总长度，那么就先把这些从缓存中取出来然后接着接收
					if (bytes_transafered < header.total_size)
					{
						// 先取出缓存
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + json_header_string_size,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// 更新
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// 重新部署监听任务, 此时数据结构不需要再解析头部，由adapt->head_parse记录已解析，recv_tmp_package记录之前接收的头部数据
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleJsonHeaderRread(ec, adapt, size);
							});
						adapt->head_parse = true;


						// 交由async_read_some，继续接收剩余数据
						return;
					}

					// 如果接收到的数据大于总长度，发生了粘包现像, 此时包中假定存在完整数据
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_LEAST__,
						adapt->recv_tmp_buff->getData(), header.msg_size);

					adapt->recv_tmp_package->getCurSize() += header.msg_size + 1;
					copy_len += header.msg_size;
					bytes_transafered -= header.msg_size;

					// 添加终止符
					static_cast<char*>(adapt->recv_tmp_package->getData())[(adapt->recv_tmp_package->getTotalSize() - 1)] = '\0';

					//将接收结果推送到队列当中
					adapt->recv_completed_queue.push(adapt->recv_tmp_package);

					// 通知接收到一个包
					this->session_notified_queue.push(adapt);
					this->notified_cond.notify_all();

					// 获取结束，重新初始化内容
					adapt->head_parse = false;
					// 重新构造结点
					adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();
					// 清空接收缓冲区
					adapt->recv_tmp_buff->clear();

					// 如果读完，则重新布置监听任务
					if (bytes_transafered <= 0)
					{
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_LEAST__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleJsonHeaderRread(ec, adapt, size);
							});
						return;
					}

					// 未读完则继续解析内容
					continue;
				}

				//已经处理完头部，处理上次未接受完的消息数据
				//接收的数据仍不足剩余未处理的
				uint32_t json_header_string_size = 0; // 头部json长度 
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeJsonHeader(json_header_string_size);
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// 如果说接受的数据还是少于所需求的
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// 重新布置监听任务
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_LEAST__),
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleJsonHeaderRread(ec, adapt, size);
						});
					return;
				}

				// 如果接受数据长度大于等于所需要的信息长度，则代表发生了粘包现象
				memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
					adapt->recv_tmp_buff->getData(), bytes_transafered);

				// 更新接收数据
				adapt->recv_tmp_package->getCurSize() += msg_remain + 1;
				bytes_transafered -= msg_remain;
				copy_len += msg_remain;

				// 添加终止符
				static_cast<char*>(adapt->recv_tmp_package->getData())[adapt->recv_tmp_package->getCurSize() + 1] = '\0';

				// 加入完成队列
				adapt->recv_completed_queue.push(adapt->recv_tmp_package);

				// 通知接收到一个包，并将session传给通知队列
				this->session_notified_queue.push(adapt);
				this->notified_cond.notify_all();

				// 重新构造节点
				adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();

				adapt->head_parse = false;
				adapt->recv_tmp_buff->clear();

				if (bytes_transafered <= 0)
				{
					// 重新布置监听任务
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__),
						[this, adapt](const EC& ec, size_t size)->void
						{
							this->handleJsonHeaderRread(ec, adapt, size);
						});
					return;
				}

				continue;
			}
		}
		void handleWrite(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transafered)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::MAsyncServerData::handleWrite", "Error Code is %d, Error Message is %s", MERROR::SEND_ERROR, ec.message().c_str());
				this->parent->earse(adapt->getUUID());
				return;
			}
			// 表示还没写完当前的buff，需要继续发送
			if (bytes_transafered + adapt->send_tmp_buff->getCurSize() < adapt->send_tmp_buff->getTotalSize())
			{
				adapt->send_tmp_buff->getCurSize() += bytes_transafered;
				adapt->socket.async_write_some(boost::asio::buffer(static_cast<char*>(adapt->send_tmp_buff->getData()) + adapt->send_tmp_buff->getCurSize(),
					adapt->send_tmp_buff->getTotalSize() - adapt->send_tmp_buff->getCurSize()),
					[this, adapt](const EC& ec, size_t size)
					{
						this->handleWrite(ec, adapt, size);
					});
			}
			else// 表示写完当前的buff，读取queue进行发送
			{
				if (adapt->send_queue.empty())
				{
					adapt->send_pending = false;
					return;
				}
				adapt->send_pending = true;

				adapt->send_tmp_buff = *adapt->send_queue.front();
				adapt->send_queue.pop();
				adapt->socket.async_write_some(boost::asio::buffer(adapt->send_tmp_buff->getData(), adapt->send_tmp_buff->getTotalSize()),
					[this, adapt](const EC& ec, size_t size)->void
					{
						this->handleWrite(ec, adapt, size);
					});
			}

		}
	public:
		MAsyncServer* parent;
		TCPAcceptor acceptor;
		std::map<std::string, NetAsyncIOAdapt> sessions;  // 会话map
		MSyncAnnularQueue<NetAsyncIOAdapt> session_notified_queue;  // 通知队列
		NotifiedFunction notified_fun;  // 通知函数
		
		std::atomic<bool> notified_thread_flag; // 通知逻辑层的原子锁
		std::thread notified_thread;  // 通知线程
		std::mutex notified_mutex;  // 通知锁
		std::condition_variable notified_cond;  // 通知条件变量

	};


	MAsyncServer::MAsyncServer(int& error_code, const MServerEndPoint& endpoint, NotifiedFunction notified_fun)
		:m_data(new MAsyncServerData(this, this->getIOContext(), endpoint, notified_fun))
	{}

	MAsyncServer::~MAsyncServer()
	{
		this->m_data->notified_thread_flag = false;
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

	int MAsyncServer::readRawPackage(NetAsyncIOAdapt adapt)
	{
		if (adapt->recv_pending)
		{
			return MERROR::READ_PENDING_NOW;
		}

		adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
			[this, adapt](const EC& ec, size_t size)->void
			{
				this->m_data->handleRawHeaderRread(ec, adapt, size);
			});

		return 0;
	}

	int MAsyncServer::readJsonPackage(NetAsyncIOAdapt adapt)
	{
		if (adapt->recv_pending)
		{
			return MERROR::READ_PENDING_NOW;
		}

		adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
			[this, adapt](const EC& ec, size_t size)->void
			{
				this->m_data->handleJsonHeaderRread(ec, adapt, size);
			});

		return 0;
	}

	int MAsyncServer::writeRawPackage(NetAsyncIOAdapt adapt, const void* data, uint32_t size)
	{
		adapt->send_queue.push(std::make_shared<MSendMsgNode<__MUZI_MMSGNODE_PACKAGE_JSONFORMAT_OFF__>>(data, size));
		if (adapt->send_pending)
		{
			return MERROR::WRITE_PENDING_NOW;
		}

		adapt->send_tmp_buff = *adapt->send_queue.front();
		adapt->send_queue.pop();
		adapt->socket.async_write_some(boost::asio::buffer(adapt->send_tmp_buff->getData(), adapt->send_tmp_buff->getTotalSize()),
			[this, adapt](const EC& ec, size_t size)->void
			{
				this->m_data->handleWrite(ec, adapt, size);
			});

		return 0;
	}

	int MAsyncServer::writeJsonPackage(NetAsyncIOAdapt adapt, const void* data, uint32_t size)
	{
		adapt->send_queue.push(std::make_shared<MSendMsgNode<__MUZI_MMSGNODE_PACKAGE_JSONFORMAT_ON__>>(data, size));
		if (adapt->send_pending)
		{
			return MERROR::WRITE_PENDING_NOW;
		}

		adapt->send_tmp_buff = *adapt->send_queue.front();
		adapt->send_queue.pop();
		adapt->socket.async_write_some(boost::asio::buffer(adapt->send_tmp_buff->getData(), adapt->send_tmp_buff->getTotalSize()),
			[this, adapt](const EC& ec, size_t size)->void
			{
				this->m_data->handleWrite(ec, adapt, size);
			});

		return 0;
	}

	int MAsyncServer::writeRawPackage(NetAsyncIOAdapt adapt, String& data)
	{
		return this->writeRawPackage(adapt, static_cast<const void*>(data.c_str()), data.size());
	}

	int MAsyncServer::writeJsonPackage(NetAsyncIOAdapt adapt, String& data)
	{
		return this->writeJsonPackage(adapt, static_cast<const void*>(data.c_str()), data.size());
	}

	MAsyncServer::NotifiedLock MAsyncServer::getNotifiedLock()
	{
		return NotifiedLock(this->m_data->notified_mutex, this->m_data->notified_cond);
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