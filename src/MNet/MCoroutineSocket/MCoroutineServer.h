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
		MCoroutineServer(int& error_code, const MServerEndPoint& endpoint);
		MCoroutineServer(int& error_code, IOContext& context, const MServerEndPoint& endpoint);
		MCoroutineServer() = delete;
		~MCoroutineServer();
	public:
		Awaitable<int> listen(int back_log);
		int accept(MCoroSessionPack& session);
		
	public:
		void startSession(MCoroSessionPack& session);
		void closeSession(MCoroSessionPack& session);
	public:

	public:
		class MCoroutineServerData* m_data;
	};
}

#endif // !__MUZI_MCOROUTINESERVER_H__
