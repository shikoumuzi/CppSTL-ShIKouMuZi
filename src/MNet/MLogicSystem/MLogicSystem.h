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

namespace MUZI::net
{
	class LogicSystem: public  singleton::MSingleton<LogicSystem>
	{
	public:
		using MsgCtrlCallBack = std::function<void(async::NetAsyncIOAdapt, const int msg_id, const std::string& msg_data)>;
	private:
		LogicSystem(async::MAsyncServer& server);
	public:
		~LogicSystem();
	private:
		void registerCallBacks(int msg_id, MsgCtrlCallBack&& callback);
		void HelloWordCallBack(async::NetAsyncIOAdapt, const int msg_id, const std::string& msg_data);
		void ctrlRecvMsg();
		void ctrlSendMsg();
	public:
		void pushToMsgQueue(const MLogicPackage& msg);

	public:
		bool isWorking();
	public:
		void notifiedFun(async::MAsyncSocket&);
	private:
		std::queue<std::shared_ptr<MLogicNode>> m_recv_msg_que;
		std::mutex m_lock;
		std::condition_variable m_cond;
		std::thread m_work_thread;
		std::atomic<bool> m_work_flag;
		std::map<int, MsgCtrlCallBack> m_fun_callback;

		async::MAsyncServer& m_server;
	};

}



#endif // !__MUZI_MLOGSYSTEM_H__
