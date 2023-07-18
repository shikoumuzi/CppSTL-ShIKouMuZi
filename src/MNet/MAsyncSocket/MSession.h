#pragma once
#ifndef __MUZI_MSYNCSOCKET_MSESSION_H__
#define __MUZI_MSYNCSOCKET_MSESSION_H__
#include<memory>
#include"MNet/MNetBase.h"
#include"MMsgNode.h"
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#define __MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__ 2048
namespace MUZI::NET::ASYNC
{

	// std::enable_shared_from_this<MSession> 用以同步引用计数
	class MSession :std::enable_shared_from_this<MSession>
	{
	public:
		friend class MAsyncSocket;
		friend class MAsyncServer;
		friend class MAsyncClient;
	private:
		static String createUUID();
	public:
		MSession(TCPSocket socket);
		~MSession();
	public:
		inline bool isWriteCompleted();
		inline bool isReadCompleted();
	public:
		const String& getUUID();
	public:
		inline TCPSocket& getSocket();
		MsgPackage getPopFrontRecvMsg();
	private:
		SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__>> send_queue;
		SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__>> recv_queue;
		SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__>> recv_completed_queue;
		TCPSocket socket;
		bool send_pending;
		bool recv_pending;
		String uuid;
	};

	using NetAsyncIOAdapt = std::shared_ptr<MSession>;


}

#endif // !__MUZI_MSYNCSOCKET_MSession_H__
