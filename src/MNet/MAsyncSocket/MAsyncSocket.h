#pragma once
#ifndef __MUZI_MASYNCSOCKET_H__
#define __MUZI_MASYNCSOCKET_H__
#include"MNet/MNetBase.h"
#include<queue>
#include"MNet/MEndPoint/MServerEndPoint.h"
#include"MNet/MEndPoint/MEndPoint.h"
#define __MUZI_MAsyncSocket_RECV_ONCE_SIZE_IN_BYTES__ 1024
namespace MUZI::NET::ASYNC
{
	class MsgNode
	{
	public:
		friend class MAsyncSocket;
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
		friend class MAsyncSocket;
		friend class MAsyncServer;
	public:
		Session(TCPSocket socket);
		~Session();
	public:
		inline bool isWriteCompleted();
		inline bool isReadCompleted();
	public:
		inline TCPSocket& Socket();
	private:
		std::queue<std::shared_ptr<MsgNode>> send_queue;
		std::queue<std::shared_ptr<MsgNode>> recv_queue;
		TCPSocket socket;
		bool send_pending;
		bool recv_pending;
	};

	using NetAsyncIOAdapt = std::shared_ptr<Session>;


	class __declspec(novtable) MAsyncSocket
	{
	public:
		class MAsyncSocketData;

	public:
		MAsyncSocket();
		~MAsyncSocket();
	public:
		IOContext& getIOContext();
	public:
		int writeToSocket(NetAsyncIOAdapt& adapt, String& data);
		int wtiteToSocket(NetAsyncIOAdapt& adapt, void* data, uint64_t size);
		int wtiteAllToSocket(NetAsyncIOAdapt& adapt, void* data, uint64_t size);

		int readFromSocket(NetAsyncIOAdapt& adapt, uint64_t size);
		int readAllFromeSocket(NetAsyncIOAdapt& adapt, uint64_t size);
	public:
		class MAsyncSocketData* m_data;

	};


}


#endif // !__MUZI_MASYNCSOCKET_H__
