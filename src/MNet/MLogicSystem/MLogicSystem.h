#pragma once
#ifndef __MUZI_MLOGSYSTEM_H__
#define __MUZI_MLOGSYSTEM_H__
#include<map>
#include"MDesignModel/MSingleton/MSingleton.h"
#include<queue>
#include"../MAsyncSocket/MAsyncServer.h"
#include"../MAsyncSocket/MAsyncSocket.h"
#include"MConfigValue.h"
#include<functional>
#include<thread>
#include<rapidjson/prettywriter.h>
#include<rapidjson/stringbuffer.h>
#include<rapidjson/rapidjson.h>
#include<rapidjson/document.h>
#include"MNet/MAsyncSocket/MSession.h"
#include<string>
#include<atomic>
#include"MLogicNode.h"
#include<signal.h>
#include<variant>
#include"MNet/MCoroutineSocket/MSession.h"
#include"MNet/MCoroutineSocket/MCoroutineSocket.h"

namespace MUZI::net
{
	template<typename T>
	concept SessionConcept = requires(T x)
	{
		std::same_as<T, async::NetAsyncIOAdapt> || std::same_as<T, coroutine::MCoroSessionPack>;
	};

	template<typename T>
	concept SocketConcpet = requires(T x)
	{
		std::same_as<T, async::MAsyncSocket> || std::same_as<T, coroutine::MCoroutineSocket>;
	};


	template<SessionConcept Session = async::MSession, SocketConcpet Socket = async::MAsyncSocket>
	class LogicSystem : public  singleton::MSingleton<LogicSystem<Session, Socket>>
	{
	public:

		using MsgCtrlCallBack = std::function<void(Session, const int msg_id, const std::string& msg_data)>;
	private:
		LogicSystem(async::MAsyncServer& server) :
			m_work_flag(true),
			m_server(&server)
		{
			this->registerCallBacks(MSG_IDS::MSG_HELLO_WORD,
				[](Session, const int msg_id, const std::string& msg_data) {});

			this->m_work_thread = std::thread(
				[this]()
				{
					this->ctrlRecvMsg();
				});
			this->m_work_thread.detach();
		}
	public:
		~LogicSystem()
		{
			this->m_work_flag = false;
			this->m_cond.notify_all();
		}
	private:
		void registerCallBacks(int msg_id, MsgCtrlCallBack&& callback)
		{
			this->m_fun_callback[msg_id] = callback;
		}
		void HelloWordCallBack(Session session_pack, const int msg_id, const std::string& msg_data)
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

			auto var_ptr = this->m_server;
			if (var_ptr == nullptr)
			{
				return;
			}
			(*var_ptr)->writePackage(session_pack, msg_data);
			

		}
		void ctrlRecvMsg()
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
						MLog::d("LogicSystem::ctrlMsg", "msg id is %d", msg_node->getPackage()->getId());
						auto call_back_iter = this->m_fun_callback.find(msg_node->getPackage()->getId());
						// 如果没找到则直接越过该消息包
						if (call_back_iter == this->m_fun_callback.end())
						{
							this->m_recv_msg_que.pop();
							continue;
						}
						// 调用对应回调函数
						call_back_iter->second(msg_node->getSession(), msg_node->getPackage()->getId(), static_cast<char*>(msg_node->getPackage()->getMsg()));
					
						this->m_recv_msg_que.pop();
					}
					break;
				}

				// 如果工作没有中止，且队列中有数据
				auto msg_node = this->m_recv_msg_que.front();
				MLog::d("LogicSystem::ctrlMsg", "msg id is %d", msg_node->getPackage()->getId());
				auto call_back_iter = this->m_fun_callback.find(msg_node->getPackage()->getId());
				// 如果没找到则直接越过该消息包
				if (call_back_iter == this->m_fun_callback.end())
				{
					this->m_recv_msg_que.pop();
					continue;
				}
				// 调用对应回调函数
				call_back_iter->second(msg_node->getSession(), msg_node->package->getId(), static_cast<char*>(msg_node->package->getMsg()));

				__MUZI_MNET_DEFAULT_SLEEP_TIME_IN_MILLISECOND_FOR_ENDLESS_LOOP__;
			}
		}
		void ctrlSendMsg()
		{}
	public:
		void pushToMsgQueue(const MLogicPackage<Session>& msg)
		{
			std::unique_lock<std::mutex> unique_lk(this->m_lock);
			this->m_recv_msg_que.push(msg);
			if (this->m_recv_msg_que.size() == 1)
			{
				this->m_cond.notify_all();
			}
		}

	public:
		bool isWorking()
		{
			return this->m_work_flag;
		}
	public:
		void notifiedFun(Socket&)
		{
			MSyncAnnularQueue<Session>& sessions_notified_queue = socket.getSessionNotifiedQueue();

			std::lock_guard<std::mutex> guard_lock(socket.getNotifiedLock().notified_mutex);
			Session* session = sessions_notified_queue.front();
			this->pushToMsgQueue(std::make_shared<MLogicNode<Session>>(*session, session->get()->getPopFrontRecvMsg()));
			sessions_notified_queue.pop();
		}
	private:
		std::queue<std::shared_ptr<MLogicNode<Session>>> m_recv_msg_que;
		std::mutex m_lock;
		std::condition_variable m_cond;
		std::thread m_work_thread;
		std::atomic<bool> m_work_flag;
		std::map<int, MsgCtrlCallBack> m_fun_callback;
		Socket m_server;
	};
}

#endif // !__MUZI_MLOGSYSTEM_H__
