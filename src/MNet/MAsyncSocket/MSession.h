#pragma once
#ifndef __MUZI_MASYNCSOCKET_MSESSION_H__
#define __MUZI_MASYNCSOCKET_MSESSION_H__
#include<memory>
#include"MNet/MNetBase.h"
#include"../MMsgNode/MMsgNode.h"
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include<queue>
#include"MSTL/h/MSyncAnnularQueue.h"
#include<mutex>
#include"../MMsgNode/MSendMsgNode.h"
#include"../MMsgNode/MRecvMsgNode.h"
#define __MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__ 2048
namespace MUZI::net::async
{

	class MAsyncSocket;


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
		MSession(IOContext& io_context);
		~MSession();
	public:
		inline bool isWriteCompleted();
		inline bool isReadCompleted();
	public:
		const String& getUUID();
	public:
		TCPSocket& getSocket();
		RecvMsgPackage getPopFrontRecvMsg();
	private:
		//SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__>> send_queue;
		//SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__ / 2>> recv_queue;
		//SpecQueue<MsgPackage, boost::lockfree::capacity<__MUZI_MASYNCSOCKET_LOCKFREE_SPSE_QUEUE_CAPACITY__ / 2>> recv_queue;
		//std::queue<MsgPackage> send_queue;
		//std::queue<MsgPackage> recv_queue;

		MSyncAnnularQueue<SendMsgPackage> send_queue;
		SendMsgPackage send_tmp_buff;
		MSyncAnnularQueue<RecvMsgPackage> recv_completed_queue;
		RecvMsgPackage recv_tmp_buff;
		RecvMsgPackage recv_tmp_package;

		TCPSocket socket;
		bool send_pending;
		bool recv_pending;
		String uuid;
		uint64_t recv_id;
		uint64_t send_id;
		bool head_parse;
		Strand m_strand;

	};

	using NetAsyncIOAdapt = std::shared_ptr<MSession>;


}
  
#endif // !__MUZI_MSYNCSOCKET_MSession_H__
