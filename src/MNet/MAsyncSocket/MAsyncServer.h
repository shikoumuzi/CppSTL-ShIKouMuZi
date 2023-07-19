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

namespace MUZI::NET::ASYNC
{
	class MAsyncServer: public MAsyncSocket
	{
	public:
		class MAsyncServerData;
	public:
		friend class MAsyncServerData;
	public:
		MAsyncServer(int& error_code, const MServerEndPoint& endpoint);
	public:
		int listen(int back_log);
		int accept(std::function<void(NetAsyncIOAdapt)>& adapt_output);
		NetAsyncIOAdapt accept(int& error_code);
	public:
		std::map<String, NetAsyncIOAdapt>& getNetAsyncIOAdapt();
	public:
		int handle_accpet(NetAsyncIOAdapt adapt, const EC& ec);
	private:
		class MAsyncServerData* m_data;
		
	};
}




#endif // !__MUZI_MASYNCSERVER_H__
