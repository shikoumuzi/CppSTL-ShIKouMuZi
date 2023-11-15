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

							// �����Ƿ�Ϊ�� �� �Ƿ���Ȼ�д���֪ͨ�е�����
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

							// �����Ƿ�Ϊ�� �� �Ƿ���Ȼ�д���֪ͨ�е�����
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
			// ��ʾͷԪ����ɴ���
			adapt->send_queue.pop();

			// �����Ϣ�������Ϊ�գ�����send_pending Ϊfalse ��������
			if (adapt->send_queue.empty())
			{
				adapt->send_pending = false;
			}
			else // �����Ϊ�����������
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

			// ����Ѿ���ɸ��ƣ���ôֱ�ӽ�������ݿ�������ɶ��е���
			adapt->recv_completed_queue.push(adapt->recv_tmp_buff);

			adapt->recv_pending = false;
			adapt->recv_tmp_buff->clear();
			// ���²��ü�������
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
			// �������ô�������
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

				// ͷ��û�н������
				if (!adapt->head_parse)
				{
					// ͷ������û�н�����ȫ,������С��ͷ���涨���ȣ� �ȸ���һ���֣�Ȼ���������
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
					// ����յ������ݱ�ͷ����,��ͷ����û�����꣨!adapt->recv_tmp_buff->getHeadParse()��
					// ͷ��ʣ��û�и��Ƶĳ���
					int head_remain = __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__ - adapt->recv_tmp_buff->getCurSize();

					// ��ͷ��ʣ�������Ȼ��浽recv_tmp_package����
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// �����Ѵ����data���Ⱥ�ʣ��δ����ĳ���
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;

					// ��ȡͷ������,�����»����
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
					adapt->recv_tmp_package->getCurSize() += head_remain;
					//if (header.msg_id > __MUZI_MMSGNODE_MSGNODE_DEFAULT_MAX_MAG_ID__)
					// ����������ض��İ�ͷid����ֱ��ɾ��
					if(header.msg_id != __MUZI_MASYNCSOCKET_SPECIFICAL_PACKAGE_HEADER_ID__)
					{
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// ��ʾͷ�����ȷǷ�
					if (header.total_size > __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						// ɾ������
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// ������յ�������С���ܳ��ȣ�˵������û����ȫ����ô���Ȱ���Щ�ӻ�����ȡ����Ȼ����Ž���
					if (bytes_transafered < header.total_size)
					{
						// ��ȡ������
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// ����
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// ���²����������, ��ʱ���ݽṹ����Ҫ�ٽ���ͷ������adapt->head_parse��¼�ѽ�����recv_tmp_package��¼֮ǰ���յ�ͷ������
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							});
						adapt->head_parse = true;


						// ����async_read_some����������ʣ������
						return;
					}

					// ������յ������ݴ����ܳ��ȣ�������ճ������, ��ʱ���мٶ�������������
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
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
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							});
						return;
					}

					// δ�����������������
					continue;
				}

				//�Ѿ�������ͷ���������ϴ�δ���������Ϣ����
				//���յ������Բ���ʣ��δ�����
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// ���˵���ܵ����ݻ��������������
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// ���²��ü�������
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleRead(ec, adapt, size);
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

				// ͷ��û�н������
				if (!adapt->head_parse)
				{
					// ͷ������û�н�����ȫ,������С��ͷ���涨���ȣ� �ȸ���һ���֣�Ȼ���������
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
					// ����յ������ݱ�ͷ����,��ͷ����û�����꣨!adapt->recv_tmp_buff->getHeadParse()��
					// ͷ��ʣ��û�и��Ƶĳ���
					int head_remain = __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__ - adapt->recv_tmp_buff->getCurSize();

					// ��ͷ��ʣ�������Ȼ��浽recv_tmp_package����
					memcpy(adapt->recv_tmp_package->getData(), adapt->recv_tmp_buff->getData(), head_remain);

					// �����Ѵ����data���Ⱥ�ʣ��δ����ĳ���
					copy_len += head_remain;
					bytes_transafered -= head_remain;
					adapt->recv_tmp_buff->getCurSize() += head_remain;

					// ��ȡͷ������,�����»����
					MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
					adapt->recv_tmp_package->getCurSize() += head_remain;
					//if (header.msg_id > __MUZI_MMSGNODE_MSGNODE_DEFAULT_MAX_MAG_ID__)
					// ����������ض��İ�ͷid����ֱ��ɾ��
					if (header.msg_id != __MUZI_MASYNCSOCKET_SPECIFICAL_PACKAGE_HEADER_ID__)
					{
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// ��ʾͷ�����ȷǷ�
					if (header.total_size > __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__)
					{
						// ɾ������
						this->sessions.erase(adapt->getUUID());
						return;
					}

					// ������յ�������С���ܳ��ȣ�˵������û����ȫ����ô���Ȱ���Щ�ӻ�����ȡ����Ȼ����Ž���
					if (bytes_transafered < header.total_size)
					{
						// ��ȡ������
						memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
							adapt->recv_tmp_buff->getData(), bytes_transafered);

						// ����
						adapt->recv_tmp_package->getCurSize() += bytes_transafered;
						adapt->recv_tmp_buff->clear();

						// ���²����������, ��ʱ���ݽṹ����Ҫ�ٽ���ͷ������adapt->head_parse��¼�ѽ�����recv_tmp_package��¼֮ǰ���յ�ͷ������
						adapt->socket.async_read_some(
							boost::asio::buffer(static_cast<char*>(adapt->recv_tmp_buff->getData()), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							boost::asio::bind_executor(adapt->m_strand,
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							}));
						adapt->head_parse = true;


						// ����async_read_some����������ʣ������
						return;
					}

					// ������յ������ݴ����ܳ��ȣ�������ճ������, ��ʱ���мٶ�������������
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + __MUZI_MMSGNODE_MSGNODE_HEADER_SIZE_IN_BYTES__,
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
						adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
							boost::asio::bind_executor(adapt->m_strand,
							[this, adapt](const EC& ec, size_t size)->void
							{
								this->handleRead(ec, adapt, size);
							}));
						return;
					}

					// δ�����������������
					continue;
				}

				//�Ѿ�������ͷ���������ϴ�δ���������Ϣ����
				//���յ������Բ���ʣ��δ�����
				MMsgNodeDataBaseMsg header = adapt->recv_tmp_package->analyzeHeader();
				int msg_remain = header.total_size - adapt->recv_tmp_package->getCurSize();

				// ���˵���ܵ����ݻ��������������
				if (bytes_transafered < msg_remain)
				{
					memcpy(static_cast<char*>(adapt->recv_tmp_package->getData()) + adapt->recv_tmp_package->getCurSize(),
						adapt->recv_tmp_buff->getData(), bytes_transafered);

					adapt->recv_tmp_package->getCurSize() += bytes_transafered;
					adapt->recv_tmp_buff->clear();

					// ���²��ü�������
					adapt->socket.async_read_some(boost::asio::buffer(adapt->recv_tmp_buff->getData(), __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__),
						boost::asio::bind_executor(adapt->m_strand,
						[this, adapt](const EC& ec, std::size_t size)->void
						{
							this->handleRead(ec, adapt, size);
						}));
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
			// ��ʾ��ûд�굱ǰ��buff����Ҫ��������
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
					boost::asio::bind_executor(adapt->m_strand,
						[this, adapt](const EC& ec, size_t size)
						{
							this->handleWrite(ec, adapt, size);
						}));
			}
		}

	public:

		std::map<std::string, NetAsyncIOAdapt> sessions;  // �Ựmap
		MSyncAnnularQueue<NetAsyncIOAdapt> session_notified_queue;  // ֪ͨ����
		NotifiedFunction notified_fun;  // ֪ͨ����

		std::atomic<bool> notified_thread_flag; // ֪ͨ�߼����ԭ����
		std::thread notified_thread;  // ֪ͨ�߳�
		std::mutex notified_mutex;  // ֪ͨ��
		std::condition_variable notified_cond;  // ֪ͨ��������

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
		// ˵����ǰ��Ȼ�ڶ�
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
		// �ú������ڸ��߼�����л�ȡ֪ͨ����֪ͨ��������
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