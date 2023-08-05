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

namespace MUZI::net::async
{
	class MAsyncServer: public MAsyncSocket
	{
	public:
		static void defalutNotifyFunction(MAsyncServer&){}
	public:
		using iterator = std::map<String, NetAsyncIOAdapt>::iterator;
		using NotifiedFunction = std::function<void(MAsyncServer&)>;
	public:
		class MAsyncServerData;
	public:
		friend class MAsyncServerData;
	public:
		MAsyncServer(int& error_code, const MServerEndPoint& endpoint, NotifiedFunction notified_fun = defalutNotifyFunction);
		~MAsyncServer();
	public:
		int listen(int back_log);
		int accept(const std::function<void(MAsyncServer&, NetAsyncIOAdapt)>& adapt_output = [](MAsyncServer&, NetAsyncIOAdapt)->void {});
		NetAsyncIOAdapt accept(int& error_code);
	public:
		int readRawPackage(NetAsyncIOAdapt adapt);
		int readJsonPackage(NetAsyncIOAdapt adapt);
	public:
		NetAsyncIOAdapt& getNetAsyncIOAdapt(String UUID);
		iterator begin();
		iterator end();
		void earse(String UUID);
		iterator earse(iterator& it);
	private:
		class MAsyncServerData* m_data;
		
	};
}




#endif // !__MUZI_MASYNCSERVER_H__
