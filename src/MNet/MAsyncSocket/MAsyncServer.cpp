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

							// �����Ƿ�Ϊ�� �� �Ƿ���Ȼ�д���֪ͨ�е�����
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
			// ���ӳɹ��˲ŷ�����ӵ���
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

				// ͷ��û�н������
				if (!adapt->head_parse)
				{
					// ͷ������û�н�����ȫ,������С��ͷ���涨���ȣ� �ȸ���һ���֣�Ȼ���������
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
					// ����յ������ݱ�ͷ����,��ͷ����û�����꣨!adapt->recv_tmp_buff->getHeadParse()��
					// ͷ��ʣ��û�и��Ƶĳ���
					int head_remain = __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__ - adapt->recv_tmp_buff->getCurSize();

					// ��ͷ��ʣ�������Ȼ��浽recv_tmp_package����
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// �����Ѵ����data���Ⱥ�ʣ��δ����ĳ���
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;

					// ��ȡͷ������,�����»����
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeRawHeader();
					adapt->recv_tmp_package->getCurSize() += head_remain;
					if (header.msg_id > __MUZI_MMSGNODE_MSGNODE_DEFAULT_MAX_MAG_ID__)
					{
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// ��ʾͷ�����ȷǷ�
					if (header.total_size > __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						// ɾ������
						this->parent->earse(adapt->getUUID());
						return;
					}

					// ������յ�������С���ܳ��ȣ�˵������û����ȫ����ô���Ȱ���Щ�ӻ�����ȡ����Ȼ����Ž���
					if (bytes_transafered < header.total_size)
					{
						// ��ȡ������
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// ����
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// ���²����������, ��ʱ���ݽṹ����Ҫ�ٽ���ͷ������adapt->head_parse��¼�ѽ�����recv_tmp_package��¼֮ǰ���յ�ͷ������
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRawHeaderRread(ec, adapt, size);
							});
						adapt->head_parse = true;


						// ����async_read_some����������ʣ������
						return;
					}

					// ������յ������ݴ����ܳ��ȣ�������ճ������, ��ʱ���мٶ�������������
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), header.msg_size);

					adapt->recv_tmp_package->getCurSize() += header.msg_size + 1;
					copy_len += header.msg_size;
					bytes_transafered -= header.msg_size;

					// �����ֹ��
					static_cast<char*>(adapt->recv_tmp_package->getData())[(adapt->recv_tmp_package->getTotalSize() - 1)] = '\0';

					//�����ս�����͵����е���
					adapt->recv_completed_queue.push(adapt->recv_tmp_package);

					// ֪ͨ���յ�һ����
					this->session_notified_queue.push(adapt);
					this->notified_cond.notify_all();

					// ��ȡ���������³�ʼ������
					adapt->head_parse = false;
					// ���¹�����
					adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();
					// ��ս��ջ�����
					adapt->recv_tmp_buff->clear();

					// ������꣬�����²��ü�������
					if (bytes_transafered <= 0)
					{
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRawHeaderRread(ec, adapt, size);
							});
						return;
					}

					// δ�����������������
					continue;
				}

				//�Ѿ�������ͷ���������ϴ�δ���������Ϣ����
				//���յ������Բ���ʣ��δ�����
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeRawHeader();
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// ���˵���ܵ����ݻ��������������
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// ���²��ü�������
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__),
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleRawHeaderRread(ec, adapt, size);
						});
					return;
				}

				// ����������ݳ��ȴ��ڵ�������Ҫ����Ϣ���ȣ����������ճ������
				memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
					adapt->recv_tmp_buff->getData(), bytes_transafered);

				// ���½�������
				adapt->recv_tmp_package->getCurSize() += msg_remain + 1;
				bytes_transafered -= msg_remain;
				copy_len += msg_remain;

				// �����ֹ��
				static_cast<char*>(adapt->recv_tmp_package->getData())[adapt->recv_tmp_package->getCurSize() + 1] = '\0';

				// ������ɶ���
				adapt->recv_completed_queue.push(adapt->recv_tmp_package);

				// ֪ͨ���յ�һ����������session����֪ͨ����
				this->session_notified_queue.push(adapt);
				this->notified_cond.notify_all();

				// ���¹���ڵ�
				adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();

				adapt->head_parse = false;
				adapt->recv_tmp_buff->clear();

				if (bytes_transafered <= 0)
				{
					// ���²��ü�������
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

				// ͷ��û�н������
				if (!adapt->head_parse)
				{
					// ͷ������û�н�����ȫ,������С��ͷ���涨���ȣ� �ȸ���һ���֣�Ȼ���������
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

					// ����յ������ݱ�ͷ����,��ͷ����û�����꣨!adapt->recv_tmp_buff->getHeadParse()��
					// ͷ������ʣ��û�и��Ƶĳ���
					int head_remain = __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_LEAST__ - adapt->recv_tmp_buff->getCurSize();

					// ��ͷ��ʣ�������Ȼ��浽recv_tmp_package����
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// �����Ѵ����data���Ⱥ�ʣ��δ����ĳ���
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;
					adapt->recv_tmp_package->getCurSize() += head_remain;

					uint32_t json_header_string_size = 0; // ͷ��json���� 
					// ��ȡͷ������,�����»����
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeJsonHeader(json_header_string_size);
					// ��ʾ�����ӽ����˵��ǽ���ʧ�ܣ������ٽ���һ��
					if (header.msg_id == -1)
					{
						// ���²����������, ��ʱ���ݽṹ����Ҫ�ٽ���ͷ������adapt->head_parse��¼�ѽ�����recv_tmp_package��¼֮ǰ���յ�ͷ������
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), 
								__MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__ - adapt->recv_tmp_package->getCurSize()),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleJsonHeaderRread(ec, adapt, size);
							});
						return;
					}

					// ��ʾͷ�����ȷǷ�
					if (header.total_size > __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__)
					{
						// ɾ������
						this->parent->earse(adapt->getUUID());
						return;
					}

					// ������յ�������С���ܳ��ȣ���ô���Ȱ���Щ�ӻ�����ȡ����Ȼ����Ž���
					if (bytes_transafered < header.total_size)
					{
						// ��ȡ������
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + json_header_string_size,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// ����
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// ���²����������, ��ʱ���ݽṹ����Ҫ�ٽ���ͷ������adapt->head_parse��¼�ѽ�����recv_tmp_package��¼֮ǰ���յ�ͷ������
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleJsonHeaderRread(ec, adapt, size);
							});
						adapt->head_parse = true;


						// ����async_read_some����������ʣ������
						return;
					}

					// ������յ������ݴ����ܳ��ȣ�������ճ������, ��ʱ���мٶ�������������
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_LEAST__,
						adapt->recv_tmp_buff->getData(), header.msg_size);

					adapt->recv_tmp_package->getCurSize() += header.msg_size + 1;
					copy_len += header.msg_size;
					bytes_transafered -= header.msg_size;

					// �����ֹ��
					static_cast<char*>(adapt->recv_tmp_package->getData())[(adapt->recv_tmp_package->getTotalSize() - 1)] = '\0';

					//�����ս�����͵����е���
					adapt->recv_completed_queue.push(adapt->recv_tmp_package);

					// ֪ͨ���յ�һ����
					this->session_notified_queue.push(adapt);
					this->notified_cond.notify_all();

					// ��ȡ���������³�ʼ������
					adapt->head_parse = false;
					// ���¹�����
					adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();
					// ��ս��ջ�����
					adapt->recv_tmp_buff->clear();

					// ������꣬�����²��ü�������
					if (bytes_transafered <= 0)
					{
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_LEAST__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleJsonHeaderRread(ec, adapt, size);
							});
						return;
					}

					// δ�����������������
					continue;
				}

				//�Ѿ�������ͷ���������ϴ�δ���������Ϣ����
				//���յ������Բ���ʣ��δ�����
				uint32_t json_header_string_size = 0; // ͷ��json���� 
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeJsonHeader(json_header_string_size);
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// ���˵���ܵ����ݻ��������������
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// ���²��ü�������
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_LEAST__),
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleJsonHeaderRread(ec, adapt, size);
						});
					return;
				}

				// ����������ݳ��ȴ��ڵ�������Ҫ����Ϣ���ȣ����������ճ������
				memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
					adapt->recv_tmp_buff->getData(), bytes_transafered);

				// ���½�������
				adapt->recv_tmp_package->getCurSize() += msg_remain + 1;
				bytes_transafered -= msg_remain;
				copy_len += msg_remain;

				// �����ֹ��
				static_cast<char*>(adapt->recv_tmp_package->getData())[adapt->recv_tmp_package->getCurSize() + 1] = '\0';

				// ������ɶ���
				adapt->recv_completed_queue.push(adapt->recv_tmp_package);

				// ֪ͨ���յ�һ����������session����֪ͨ����
				this->session_notified_queue.push(adapt);
				this->notified_cond.notify_all();

				// ���¹���ڵ�
				adapt->recv_tmp_package = std::make_shared<RawMRecvMsgNode>();

				adapt->head_parse = false;
				adapt->recv_tmp_buff->clear();

				if (bytes_transafered <= 0)
				{
					// ���²��ü�������
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
			// ��ʾ��ûд�굱ǰ��buff����Ҫ��������
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
			else// ��ʾд�굱ǰ��buff����ȡqueue���з���
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
		std::map<std::string, NetAsyncIOAdapt> sessions;  // �Ựmap
		MSyncAnnularQueue<NetAsyncIOAdapt> session_notified_queue;  // ֪ͨ����
		NotifiedFunction notified_fun;  // ֪ͨ����
		
		std::atomic<bool> notified_thread_flag; // ֪ͨ�߼����ԭ����
		std::thread notified_thread;  // ֪ͨ�߳�
		std::mutex notified_mutex;  // ֪ͨ��
		std::condition_variable notified_cond;  // ֪ͨ��������

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