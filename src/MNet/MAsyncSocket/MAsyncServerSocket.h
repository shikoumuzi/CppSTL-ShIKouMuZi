#pragma once
#ifndef __MUZI_MASYNCSERVERSOCKET_H__
#define __MUZI_MASYNCSERVERSOCKET_H__
#include"MNet/MNetBase.h"
#include<queue>
#include"MNet/MEndPoint/MServerEndPoint.h"
namespace MUZI::NET::ASYNC
{
	class MsgNode
	{
	public:
		friend class MAsyncServerSocket;
	public:
		class MsgNodeData;
	public:
		MsgNode(void* data, uint64_t size) :data(static_cast<void*>(data)), total_size(size),cur_size(0) {}
		~MsgNode() { data = nullptr; }
	private:
		void* data;
		uint64_t cur_size;
		uint64_t total_size;
	};

	class Session
	{
	public:
		friend class MAsyncServerSocket;
	public:
		Session(TCPSocket socket);
		~Session();
	public:
		bool isComplete() { return send_pending; }
	private:
		std::queue<std::shared_ptr<MsgNode>> msg_queue;
		TCPSocket socket;
		bool send_pending;
	};

	using NetAsyncIOAdapt = std::shared_ptr<Session>;


	class MAsyncServerSocket
	{
	public:
		class MAsyncServerSocketData;

	public:
		MAsyncServerSocket(MServerEndPoint& endpoint);
		~MAsyncServerSocket();
	public:
		int listen(int back_log);
		NetAsyncIOAdapt accept(int& ec);

	public:
		int writeToSocket(NetAsyncIOAdapt& adapt, String& data);
		int wtiteToSocket(NetAsyncIOAdapt& adapt, void* data, uint64_t size);
		int wtiteAllToSocket(NetAsyncIOAdapt& adapt, void* data, uint64_t size);

		int readFromSocket(NetAsyncIOAdapt& adapt);
		int readAllFromeSocket(NetAsyncIOAdapt& adapt);
	public:
		class MAsyncServerSocketData* m_data;

	};


}


#endif // !__MUZI_MASYNCSOCKET_H__
