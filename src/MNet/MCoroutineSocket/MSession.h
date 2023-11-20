#pragma once
#ifndef __MUZI_COROUTINE_MSESSION_H__
#define __MUZI_COROUTINE_MSESSION_H__
#include<memory>
#include"MNet/MNetBase.h"
#include"MNet/MMsgNode/MMsgNode.h"
#include"MNet/MMsgNode/MRecvMsgNode.h"
#include"MNet/MMsgNode/MSendMsgNode.h"
#include"MSTL/h/MSyncAnnularQueue.h"

namespace MUZI::net::coroutine
{
	class MSession
	{
	public:
		friend class MCoroutineSocket;
		friend class MCoroutineServer;
		friend class MCoroutineClient;
	public:
		String createUUID();
	public:
		inline bool isWriteCompleted();
		inline bool isReadCompleted();
		inline bool isClose();
	public:
		const String& getUUID();
	public:
		MSession(IOContext& io_context);
		~MSession();
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
		bool close_flag;
	};

	using MCoroSessionPack = std::shared_ptr<MSession>;
}


#endif // !__MUZI_COROUTINE_MSESSION_H__
