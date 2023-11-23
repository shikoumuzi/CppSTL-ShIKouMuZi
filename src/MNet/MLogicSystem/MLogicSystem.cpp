#include "MLogicSystem.h"
#include<cstring>
#include"MNet/MNetBase.h"
#include"MNet/MAsyncSocket/MSession.h"

namespace MUZI::net
{
	LogicSystem::LogicSystem(async::MAsyncServer& server) :
		m_work_flag(true),
		m_server(&server)
	{
		this->registerCallBacks(MSG_IDS::MSG_HELLO_WORD,
			[](std::variant<async::NetAsyncIOAdapt, coroutine::MCoroSessionPack>, const int msg_id, const std::string& msg_data) {});

		this->m_work_thread = std::thread(
			[this]()
			{
				this->ctrlRecvMsg();
			});
		this->m_work_thread.detach();
	}
	LogicSystem::~LogicSystem()
	{
		this->m_work_flag = false;
		this->m_cond.notify_all();
	}

	void LogicSystem::registerCallBacks(int msg_id, MsgCtrlCallBack&& callback)
	{
		this->m_fun_callback[msg_id] = callback;
	}

	void LogicSystem::HelloWordCallBack(std::variant<async::NetAsyncIOAdapt, coroutine::MCoroSessionPack> session_pack, const int msg_id, const std::string& msg_data)
	{
		rapidjson::Document doc;
		if (!doc.Parse(msg_data.c_str()).HasParseError())
		{
			if (doc.HasMember("id") && doc.IsInt())
			{
				printf("msg_id is %d,", doc["id"].GetInt());
			}
			if (doc.HasMember("data") && doc.IsString())
			{
				printf("data is %s.", doc["data"].GetString());
			}
		}
		if (this->m_server.index() == 0 and session_pack.index() == 0)
		{
			auto var_ptr = std::get_if<0>(&this->m_server);
			if (var_ptr == nullptr)
			{
				return;
			}
			(*var_ptr)->writePackage(std::get<0>(session_pack), msg_data);
		}
		else if (this->m_server.index() == 1 and this->m_server.index() == 1)
		{
			auto var_ptr = std::get_if<1>(&this->m_server);
			if (var_ptr == nullptr)
			{
				return;
			}
			(*var_ptr)->writeToSocket(std::get<1>(session_pack), msg_data, msg_id);
		}
	}

	void LogicSystem::ctrlRecvMsg()
	{
		while (this->m_work_flag)
		{
			std::unique_lock<std::mutex> unique_lk(this->m_lock);

			// 直到队列不为空才会跳过条件变量
			while (this->m_recv_msg_que.empty() && this->m_work_flag)
			{
				this->m_cond.wait(unique_lk);
			}

			// 如果为停止工作状态则要处理完所有的信息
			if (!this->m_work_flag)
			{
				while (!this->m_recv_msg_que.empty())
				{
					auto msg_node = this->m_recv_msg_que.front();
					MLog::d("LogicSystem::ctrlMsg", "msg id is %d", msg_node->package->getId());
					auto call_back_iter = this->m_fun_callback.find(msg_node->package->getId());
					// 如果没找到则直接越过该消息包
					if (call_back_iter == this->m_fun_callback.end())
					{
						this->m_recv_msg_que.pop();
						continue;
					}
					// 调用对应回调函数
					call_back_iter->second(msg_node->adapt, msg_node->package->getId(), static_cast<char*>(msg_node->package->getMsg()));

					this->m_recv_msg_que.pop();
				}
				break;
			}

			// 如果工作没有中止，且队列中有数据
			auto msg_node = this->m_recv_msg_que.front();
			MLog::d("LogicSystem::ctrlMsg", "msg id is %d", msg_node->package->getId());
			auto call_back_iter = this->m_fun_callback.find(msg_node->package->getId());
			// 如果没找到则直接越过该消息包
			if (call_back_iter == this->m_fun_callback.end())
			{
				this->m_recv_msg_que.pop();
				continue;
			}
			// 调用对应回调函数
			call_back_iter->second(msg_node->adapt, msg_node->package->getId(), static_cast<char*>(msg_node->package->getMsg()));

			__MUZI_MNET_DEFAULT_SLEEP_TIME_IN_MILLISECOND_FOR_ENDLESS_LOOP__;
		}
	}

	void LogicSystem::ctrlSendMsg()
	{
	}

	void LogicSystem::pushToMsgQueue(const MLogicPackage& msg)
	{
		std::unique_lock<std::mutex> unique_lk(this->m_lock);
		this->m_recv_msg_que.push(msg);
		if (this->m_recv_msg_que.size() == 1)
		{
			this->m_cond.notify_all();
		}
	}

	bool LogicSystem::isWorking()
	{
		return this->m_work_flag;
	}

	void LogicSystem::notifiedFun(async::MAsyncSocket& socket)
	{
		MSyncAnnularQueue<async::NetAsyncIOAdapt>& sessions_notified_queue = socket.getSessionNotifiedQueue();

		std::lock_guard<std::mutex> guard_lock(socket.getNotifiedLock().notified_mutex);
		async::NetAsyncIOAdapt* adapt = sessions_notified_queue.front();
		this->pushToMsgQueue(std::make_shared<MLogicNode>(*adapt, adapt->get()->getPopFrontRecvMsg()));
		sessions_notified_queue.pop();
	}
	void LogicSystem::notifiedFun(coroutine::MCoroutineSocket&)
	{
	}
}