#pragma once
#ifndef __MUZI_MASYNCSERVER_H__
#define __MUZI_MASYNCSERVER_H__
#include"MNet/MNetBase.h"
#include"MAsyncSocket.h"
#include"MBase/MError.h"
#include"MLog/MLog.h"
#include<map>
#include<string>
#include<functional>
#include<mutex>
#include<condition_variable>

namespace MUZI::net::async
{

	class MAsyncServer: public MAsyncSocket
	{
	public:
		struct NotifiedLock
		{
			std::mutex& notified_mutex;  // 通知锁
			std::condition_variable& notified_cond;  // 通知条件变量
		};
	public:
		static void defalutNotifyFunction(MAsyncSocket&){}
	public:
		using iterator = std::map<String, NetAsyncIOAdapt>::iterator;
		//using NotifiedFunction = std::function<void(MAsyncServer&)>;
	public:
		class MAsyncServerData;
	public:
		friend class MAsyncServerData;
	public:
		MAsyncServer(int& error_code, const MServerEndPoint& endpoint, NotifiedFunction notified_fun = defalutNotifyFunction);
		MAsyncServer() = delete;
		~MAsyncServer();
	public:
		int listen(int back_log);
		int accept(const std::function<void(MAsyncServer&, NetAsyncIOAdapt)>& adapt_output = [](MAsyncServer&, NetAsyncIOAdapt)->void {});
		NetAsyncIOAdapt accept(int& error_code);

	private:
		class MAsyncServerData* m_data;
		
	};
}




#endif // !__MUZI_MASYNCSERVER_H__
