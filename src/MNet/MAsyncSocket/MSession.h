#pragma once
#ifndef __MUZI_MSYNCSOCKET_MSESSION_H__
#define __MUZI_MSYNCSOCKET_MSESSION_H__
#include<memory>
#include"MNet/MNetBase.h"
#include"MMsgNode.h"
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include<queue>
#include"MSTL/h/MSyncAnnularQueue.h"
#include<mutex>

#define __MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__ 2048
namespace MUZI::net::async
{

	// std::enable_shared_from_this<MSession> 用以同步引用计数
	class MSession
	{
	public:
		friend class MAsyncSocket;
		friend class MAsyncServer;
		friend class MAsyncClient;
	private:
		static String createUUID();
		//static MsgPackage null;
	public:
		MSession(TCPSocket socket);
		~MSession();
	public:
		inline bool isWriteCompleted();
		inline bool isReadCompleted();
	public:
		const String& getUUID();
	public:
		TCPSocket& getSocket();
		MsgPackage getPopFrontRecvMsg();
	private:
		//SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__>> send_queue;
		//SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__ / 2>> recv_queue;
		//SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__ / 2>> recv_queue;
		//std::queue<MsgPackage> send_queue;
		//std::queue<MsgPackage> recv_queue;

		MSyncAnnularQueue<MsgPackage> send_queue;
		MSyncAnnularQueue<MsgPackage> recv_queue;
		MSyncAnnularQueue<MsgPackage> recv_completed_queue;

		TCPSocket socket;
		bool send_pending;
		bool recv_pending;
		String uuid;
		std::mutex send_lock;
	};

	using NetAsyncIOAdapt = std::shared_ptr<MSession>;


}

#endif // !__MUZI_MSYNCSOCKET_MSession_H__
