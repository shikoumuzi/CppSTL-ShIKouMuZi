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
		class MAsyncServerData;
	public:
		friend class MAsyncServerData;
	public:
		MAsyncServer(int& error_code, const MServerEndPoint& endpoint);
		~MAsyncServer();
	public:
		int listen(int back_log);
		int accept(const std::function<void(MAsyncServer&, NetAsyncIOAdapt)>& adapt_output = [](NetAsyncIOAdapt)->void {});
		NetAsyncIOAdapt accept(int& error_code);
	public:
		std::map<String, NetAsyncIOAdapt>& getNetAsyncIOAdapt();
		void earse(String UUID);
	private:
		class MAsyncServerData* m_data;
		
	};
}




#endif // !__MUZI_MASYNCSERVER_H__
