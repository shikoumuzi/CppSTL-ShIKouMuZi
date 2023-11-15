#include"MAsyncSocket.h"
#include<functional>
#include"MLog/MLog.h"
#include<map>
#include"MSignal/MSignalUtils.h"

namespace MUZI::net::async
{

	class MAsyncSocket::MAsyncSocketData
	{
	public:
		MAsyncSocketData(MAsyncSocket* parent, NotifiedFunction notified_function)
			:parent(parent),
			notified_fun(notified_function),
			notified_thread_flag(true),
			io_context(new IOContext()),
			m_new_io_context_flag(true)
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
							//__MUZI_MNET_DEFAULT_SLEEP_TIME_IN_MILLISECOND_FOR_ENDLESS_LOOP__;
						}
					}));
			//this->notified_thread.detach();
		}
		MAsyncSocketData(MAsyncSocket* parent, IOContext& iocontext, NotifiedFunction notified_function)
			:parent(parent),
			notified_fun(notified_function),
			notified_thread_flag(true),
			io_context(&iocontext),
			m_new_io_context_flag(false)
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
							//__MUZI_MNET_DEFAULT_SLEEP_TIME_IN_MILLISECOND_FOR_ENDLESS_LOOP__;
						}
					}));
			//this->notified_thread.detach();
		}

		~MAsyncSocketData()
		{
			this->io_context->stop();
			if (this->m_new_io_context_flag == true)
			{
				delete this->io_context;
			}
			this->io_context = nullptr;
			this->parent = nullptr;
		}

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
		
	public:
		void handleRead(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transafered)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::MAsyncServerData::handleRread", "Error Code is %d, Error Message is %s", MERROR::RECV_ERROR, ec.message().c_str());
				this->sessions.erase(adapt->getUUID());
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
					if (bytes_transafered + adapt->recv_tmp_buff->getCurSize() < __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							});

						return;
					}
					// 如果收到的数据比头部多,但头部又没处理完（!adapt->recv_tmp_buff->getHeadParse()）
					// 头部剩余没有复制的长度
					int head_remain = __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__ - adapt->recv_tmp_buff->getCurSize();

					// 将头部剩余数据先缓存到recv_tmp_package当中
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// 更新已处理的data长度和剩余未处理的长度
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;

					// 获取头部数据,并更新缓存包
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
					adapt->recv_tmp_package->getCurSize() += head_remain;
					//if (header.msg_id > __MUZI_MMSGNODE_MSGNODE_DEFAULT_MAX_MAG_ID__)
					// 如果不符合特定的包头id，则直接删除
					if(header.msg_id != __MUZI_MASYNCSOCKET_SPECIFICAL_PACKAGE_HEADER_ID__)
					{
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// 表示头部长度非法
					if (header.total_size > __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						// 删除链接
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// 如果接收到的数据小于总长度，说明数据没有收全，那么就先把这些从缓存中取出来然后接着接收
					if (bytes_transafered < header.total_size)
					{
						// 先取出缓存
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// 更新
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// 重新部署监听任务, 此时数据结构不需要再解析头部，由adapt->head_parse记录已解析，recv_tmp_package记录之前接收的头部数据
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							});
						adapt->head_parse = true;


						// 交由async_read_some，继续接收剩余数据
						return;
					}

					// 如果接收到的数据大于总长度，发生了粘包现像, 此时包中假定存在完整数据
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
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
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							});
						return;
					}

					// 未读完则继续解析内容
					continue;
				}

				//已经处理完头部，处理上次未接受完的消息数据
				//接收的数据仍不足剩余未处理的
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// 如果说接受的数据还是少于所需求的
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// 重新布置监听任务
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleRead(ec, adapt, size);
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
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
						[this, adapt](const EC& ec, size_t size)->void
						{
							this->handleRead(ec, adapt, size);
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
				this->sessions.erase(adapt->getUUID());
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
		void handleReadWithStrand(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transafered)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::MAsyncServerData::handleRread", "Error Code is %d, Error Message is %s", MERROR::RECV_ERROR, ec.message().c_str());
				this->sessions.erase(adapt->getUUID());
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
					if (bytes_transafered + adapt->recv_tmp_buff->getCurSize() < __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							boost::asio::bind_executor(adapt->m_strand,
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							}));

						return;
					}
					// 如果收到的数据比头部多,但头部又没处理完（!adapt->recv_tmp_buff->getHeadParse()）
					// 头部剩余没有复制的长度
					int head_remain = __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__ - adapt->recv_tmp_buff->getCurSize();

					// 将头部剩余数据先缓存到recv_tmp_package当中
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// 更新已处理的data长度和剩余未处理的长度
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;

					// 获取头部数据,并更新缓存包
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
					adapt->recv_tmp_package->getCurSize() += head_remain;
					//if (header.msg_id > __MUZI_MMSGNODE_MSGNODE_DEFAULT_MAX_MAG_ID__)
					// 如果不符合特定的包头id，则直接删除
					if (header.msg_id != __MUZI_MASYNCSOCKET_SPECIFICAL_PACKAGE_HEADER_ID__)
					{
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// 表示头部长度非法
					if (header.total_size > __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						// 删除链接
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// 如果接收到的数据小于总长度，说明数据没有收全，那么就先把这些从缓存中取出来然后接着接收
					if (bytes_transafered < header.total_size)
					{
						// 先取出缓存
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// 更新
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// 重新部署监听任务, 此时数据结构不需要再解析头部，由adapt->head_parse记录已解析，recv_tmp_package记录之前接收的头部数据
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							boost::asio::bind_executor(adapt->m_strand,
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							}));
						adapt->head_parse = true;


						// 交由async_read_some，继续接收剩余数据
						return;
					}

					// 如果接收到的数据大于总长度，发生了粘包现像, 此时包中假定存在完整数据
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
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
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							boost::asio::bind_executor(adapt->m_strand,
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							}));
						return;
					}

					// 未读完则继续解析内容
					continue;
				}

				//已经处理完头部，处理上次未接受完的消息数据
				//接收的数据仍不足剩余未处理的
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// 如果说接受的数据还是少于所需求的
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// 重新布置监听任务
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
						boost::asio::bind_executor(adapt->m_strand,
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleRead(ec, adapt, size);
						}));
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
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
						boost::asio::bind_executor(adapt->m_strand,
						[this, adapt](const EC& ec, size_t size)->void
						{
							this->handleRead(ec, adapt, size);
						}));
					return;
				}

				continue;
			}
		}
		void handleWriteWithStrand(const EC& ec, NetAsyncIOAdapt adapt, std::size_t bytes_transafered)
		{
			if (ec.value() != 0)
			{
				MLog::w("MAsyncServer::MAsyncServerData::handleWrite", "Error Code is %d, Error Message is %s", MERROR::SEND_ERROR, ec.message().c_str());
				this->sessions.erase(adapt->getUUID());
				return;
			}
			// 表示还没写完当前的buff，需要继续发送
			if (bytes_transafered + adapt->send_tmp_buff->getCurSize() < adapt->send_tmp_buff->getTotalSize())
			{
				adapt->send_tmp_buff->getCurSize() += bytes_transafered;
				adapt->socket.async_write_some(boost::asio::buffer(static_cast<char*>(adapt->send_tmp_buff->getData()) + adapt->send_tmp_buff->getCurSize(),
					adapt->send_tmp_buff->getTotalSize() - adapt->send_tmp_buff->getCurSize()),
					boost::asio::bind_executor(adapt->m_strand,
					[this, adapt](const EC& ec, size_t size)
					{
						this->handleWrite(ec, adapt, size);
					}));
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
					boost::asio::bind_executor(adapt->m_strand,
						[this, adapt](const EC& ec, size_t size)
						{
							this->handleWrite(ec, adapt, size);
						}));
			}
		}

	public:

		std::map<std::string, NetAsyncIOAdapt> sessions;  // 会话map
		MSyncAnnularQueue<NetAsyncIOAdapt> session_notified_queue;  // 通知队列
		NotifiedFunction notified_fun;  // 通知函数

		std::atomic<bool> notified_thread_flag; // 通知逻辑层的原子锁
		std::thread notified_thread;  // 通知线程
		std::mutex notified_mutex;  // 通知锁
		std::condition_variable notified_cond;  // 通知条件变量

		MAsyncSocket* parent;
		IOContext* io_context;
		bool m_new_io_context_flag;

	};



	MAsyncSocket::MAsyncSocket(NotifiedFunction notified_function):m_data(new MAsyncSocketData(this, notified_function))
	{
		//signal::MSignalUtils::addFunBeforeSignalTrigger(
		//	[this]() 
		//	{
		//	
		//	});
		signal::MSignalUtils::addFunWhenSignalTrigger(
			[this]()
			{
				this->~MAsyncSocket();
			},
			{SIGINT, SIGTERM});
		signal::MSignalUtils::start();
	}

	MAsyncSocket::MAsyncSocket(IOContext& io_context, NotifiedFunction notified_function)
		:m_data(new MAsyncSocketData(this, io_context, notified_function))
	{
		signal::MSignalUtils::addFunWhenSignalTrigger(
			[this]()
			{
				this->~MAsyncSocket();
			},
			{ SIGINT, SIGTERM });
		signal::MSignalUtils::start();
	}

	MAsyncSocket::~MAsyncSocket()
	{
		this->m_data->notified_thread_flag = false;
		this->m_data->io_context->stop();
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	IOContext& MAsyncSocket::getIOContext()
	{
		return *(this->m_data->io_context);
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

	int MAsyncSocket::readAllFromSocket(NetAsyncIOAdapt adapt, uint32_t size)
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

	int MAsyncSocket::readPackage(NetAsyncIOAdapt adapt)
	{
		if (adapt->recv_pending)
		{
			return MERROR::READ_PENDING_NOW;
		}

		adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
			[this, adapt](const EC& ec, size_t size)->void
			{
				this->m_data->handleRead(ec, adapt, size);
			});

		return 0;
	}

	int MAsyncSocket::writePackage(NetAsyncIOAdapt adapt, const void* data, uint32_t size)
	{
		adapt->send_queue.push(std::make_shared<MSendMsgNode>(data, size));
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

	int MAsyncSocket::writePackage(NetAsyncIOAdapt adapt, const String& data)
	{
		return this->writePackage(adapt, static_cast<const void*>(data.c_str()), data.size());
	}

	/// @brief this function is an improved version of readPackage used with io_pool
	/// @param adapt A shared pointer packet MSession
	/// @return 0 if success, MERROR::READ_PENDING_NOW if this function is reading in other thread 
	int MAsyncSocket::readPackageWithStrand(NetAsyncIOAdapt adapt)
	{
		if (adapt->recv_pending)
		{
			return MERROR::READ_PENDING_NOW;
		}

		adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
			boost::asio::bind_executor(adapt->m_strand, 
				[this, adapt](const EC& ec, size_t size)->void
				{
					this->m_data->handleReadWithStrand(ec, adapt, size);
				}));

		return 0;
	}

	/// @brief this function is an improved version of writePackage used with io_pool
	/// @param adapt A shared pointer packet MSession
	/// @param data can transfer parameters include char*, char[] or other data struct witch can cast to void *
	/// @param size data size in bytes
	/// @return 0 if success, MERROR::WRITE_PENDING_NOW if this function is reading in other thread but data in write queue
	int MAsyncSocket::writePackageWithStrand(NetAsyncIOAdapt adapt, const void* data, uint32_t size)
	{
		adapt->send_queue.push(std::make_shared<MSendMsgNode>(data, size));
		if (adapt->send_pending)
		{
			return MERROR::WRITE_PENDING_NOW;
		}

		adapt->send_tmp_buff = *adapt->send_queue.front();
		adapt->send_queue.pop();
		adapt->socket.async_write_some(boost::asio::buffer(adapt->send_tmp_buff->getData(), adapt->send_tmp_buff->getTotalSize()),
			boost::asio::bind_executor(adapt->m_strand,
				[this, adapt](const EC& ec, size_t size)->void
				{
					this->m_data->handleWriteWithStrand(ec, adapt, size);
				}));

		return 0;
		return 0;
	}

	int MAsyncSocket::writePackageWithStrand(NetAsyncIOAdapt adapt, const String& data)
	{
		return this->writePackageWithStrand(adapt, data.c_str(), data.size());
	}

	MAsyncSocket::NotifiedLock MAsyncSocket::getNotifiedLock()
	{
		// 该函数用于给逻辑层进行获取通知锁和通知条件变量
		return NotifiedLock(this->m_data->notified_mutex, this->m_data->notified_cond);
	}

	MAsyncSocket::iterator MAsyncSocket::begin()
	{
		return this->m_data->sessions.begin();
	}

	MAsyncSocket::iterator MAsyncSocket::end()
	{
		return this->m_data->sessions.end();
	}

	NetAsyncIOAdapt& MAsyncSocket::getNetAsyncIOAdapt(String UUID)
	{
		return this->m_data->sessions.at(UUID);
	}

	Map<String, NetAsyncIOAdapt>& MAsyncSocket::getSessions()
	{
		return this->m_data->sessions;
	}

	MSyncAnnularQueue<NetAsyncIOAdapt>& MAsyncSocket::getSessionNotifiedQueue()
	{
		return this->m_data->session_notified_queue;
	}

	void MAsyncSocket::erase(String UUID)
	{
		this->m_data->sessions.erase(UUID);
	}

	MAsyncSocket::iterator MAsyncSocket::erase(iterator& it)
	{
		return this->m_data->sessions.erase(it);
	}

	int MAsyncSocket::splitSendPackage(NetAsyncIOAdapt adapt, void* data, uint32_t size, uint32_t id)
	{
		uint32_t capacity = (size / __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__) + 1;
		uint32_t i = 0;
		for (; i < capacity - 1; ++i)
		{
			SendMsgPackage tmp_package(
				new MSendMsgNode(static_cast<char*>(data) + i * __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, id + 0));

			adapt->send_queue.push(tmp_package);
		}
		SendMsgPackage tmp_package(
			new MSendMsgNode(static_cast<char*>(data) + i * __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, id + 0));
		adapt->send_queue.push(tmp_package);

		return 0;
	}

	void MAsyncSocket::run()
	{
		this->m_data->io_context->run();
	}


}