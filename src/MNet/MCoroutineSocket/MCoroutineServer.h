#pragma once
#ifndef __MUZI_MCOROUTINESERVER_H__
#define __MUZI_MCOROUTINESERVER_H__
#include"MCoroutineSocket.h"
#include"MNet/MEndPoint/MServerEndPoint.h"

namespace MUZI::net::coroutine
{
	class MCoroutineServer : public MCoroutineSocket
	{
	public:
		using AcceptCallBack = std::function<void(MCoroutineServer&, MCoroSessionPack)>;
	public:
		MCoroutineServer(int& error_code, const MServerEndPoint& endpoint, NotifiedFunction notified_function);
		MCoroutineServer(int& error_code, IOContext& context, const MServerEndPoint& endpoint, NotifiedFunction notified_function);
		MCoroutineServer() = delete;
		~MCoroutineServer();
	public:
		int listen(int back_log);
		int accept(MCoroSessionPack& session, AcceptCallBack& callback);
	public:
		class MCoroutineServerData* m_data;
	};
}

#endif // !__MUZI_MCOROUTINESERVER_H__
