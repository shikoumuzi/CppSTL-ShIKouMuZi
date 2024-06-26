#pragma once
#ifndef __MUZI_MASYNCSOCKET_H__
#define __MUZI_MASYNCSOCKET_H__
#include"MNet/MNetBase.h"
#include<queue>
#include"MNet/MEndPoint/MServerEndPoint.h"
#include"MNet/MEndPoint/MEndPoint.h"
#include<boost/uuid/uuid.hpp>
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include"../MMsgNode/MMsgNode.h"
#include"../MMsgNode/MRecvMsgNode.h"
#include"../MMsgNode/MSendMsgNode.h"
#include"MSession.h"
#include<functional>
#include<map>
#include"../MError.h"
#include"MSTL/include/MSyncAnnularQueue.h"

namespace MUZI::net::async
{
	class MAsyncSocket
	{
	public:
		class MAsyncSocketData;
	public:
		using RawMRecvMsgNode = MRecvMsgNode;
		using JsonMRecvMsgNode = MRecvMsgNode;
		using MIterator = std::map<String, NetAsyncIOAdapt>::iterator;
		using NotifiedFunction = std::function<void(MAsyncSocket&)>;
	public:
		struct NotifiedLock
		{
			std::mutex& notified_mutex;  // 通知锁
			std::condition_variable& notified_cond;  // 通知条件变量
		};
	public:
		static void defalutNotifyFunction(MAsyncSocket&) {}
	public:
		MAsyncSocket(NotifiedFunction notified_function);
		MAsyncSocket(IOContext& io_context, NotifiedFunction notified_function);
		~MAsyncSocket();
	public:
		IOContext& getIOContext();
	public:
		int writeToSocket(NetAsyncIOAdapt adapt, String& data, int msg_id = 0);
		int wtiteToSocket(NetAsyncIOAdapt adapt, void* data, uint32_t size, int msg_id = 0);
		int wtiteAllToSocket(NetAsyncIOAdapt adapt, void* data, uint32_t size, int msg_id = 0);

		int readFromSocket(NetAsyncIOAdapt adapt, uint32_t size = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__);
		int readAllFromSocket(NetAsyncIOAdapt adapt, uint32_t size = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__);
	public:
		int readPackage(NetAsyncIOAdapt adapt);
	public:
		int writePackage(NetAsyncIOAdapt adapt, const void* data, uint32_t size, int msg_id = 0);
		int writePackage(NetAsyncIOAdapt adapt, const String& data, int msg_id = 0);

	public:
		int readPackageWithStrand(NetAsyncIOAdapt adapt);
		int writePackageWithStrand(NetAsyncIOAdapt adapt, const void* data, uint32_t size, int msg_id = 0);
		int writePackageWithStrand(NetAsyncIOAdapt adapt, const String& data, int msg_id = 0);
	public:
		NotifiedLock getNotifiedLock();
		NetAsyncIOAdapt& getNetAsyncIOAdapt(String UUID);
		Map<String, NetAsyncIOAdapt>& getSessions();
		MSyncAnnularQueue<NetAsyncIOAdapt>& getSessionNotifiedQueue();

	public:

		MIterator begin();
		MIterator end();
		void erase(String UUID);
		MIterator erase(MIterator& it);
	public:
		int splitSendPackage(NetAsyncIOAdapt adapt, void* data, uint32_t size, uint32_t id);
	public:
		void run();
	public:
		class MAsyncSocketData* m_data;
	};
}

#endif // !__MUZI_MASYNCSOCKET_H__
