#pragma once
#ifndef __MUZI_MASYNCSERVERSOCKET_H__
#define __MUZI_MASYNCSERVERSOCKET_H__
#include"MNet/MNetBase.h"
#include<queue>
#include"MNet/MEndPoint/MServerEndPoint.h"
#define __MUZI_MASYNCSERVERSOCKET_RECV_ONCE_SIZE_IN_BYTES__ 1024
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
		inline bool isWriteCompleted();
		inline bool isReadCompleted();
	private:
		std::queue<std::shared_ptr<MsgNode>> send_queue;
		std::shared_ptr<MsgNode> recv_queue;
		TCPSocket socket;
		bool send_pending;
		bool recv_pending;
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

		int readFromSocket(NetAsyncIOAdapt& adapt, uint64_t size);
		int readAllFromeSocket(NetAsyncIOAdapt& adapt, uint64_t size);
	public:
		class MAsyncServerSocketData* m_data;

	};


}


#endif // !__MUZI_MASYNCSOCKET_H__
