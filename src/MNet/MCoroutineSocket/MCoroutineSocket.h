#pragma once
#ifndef __MUZI_MCOROUTINESOCKET_H__
#define __MUZI_MCOROUTINESOCKET_H__
#include<boost/asio.hpp>
#include"MNet/MNetBase.h"
#include<coroutine>
#include"MSession.h"

namespace MUZI::net::coroutine
{
	class MCoroutineSocket
	{
	public:
		struct NotifiedLock
		{
			std::mutex& notified_mutex;  // 通知锁
			std::condition_variable& notified_cond;  // 通知条件变量
		};
	public:
		template<typename T>
		using Awaitable = boost::asio::awaitable<T>;
		using MIterator = std::map<String, MCoroSessionPack>::iterator;
		using NotifiedFunction = std::function<void(MCoroutineSocket&)>;
	public:
		MCoroutineSocket(NotifiedFunction notified_function);
		MCoroutineSocket(NotifiedFunction notified_function, IOContext& io_context);
	public:
		IOContext& getIOContext();
		NotifiedLock getNotifiedLock();
		MSyncAnnularQueue<MCoroSessionPack>& getSessionNotifiedQueue();
	public:
		MCoroSessionPack& getSession(const String& uuid);
		Map<String, MCoroSessionPack>& getSessions();
	public:
		Awaitable<int> readFromSocket(MCoroSessionPack& session);
		Awaitable<int> writeToSocket(MCoroSessionPack& session, const void* data, size_t data_sze, int msg_id = 0);
		Awaitable<int> writeToSocket(MCoroSessionPack& session, const std::string& data, int msg_id = 0);
	public:
		void run();
	public:
		MIterator begin();
		MIterator end();
		void erase(String UUID);
		MIterator erase(MIterator& it);
		void closeSession(MCoroSessionPack& session);
	public:
		class MCoroutineSocketData* m_data;
	};
}

#endif // !__MUZI_MCOROUTINESOCKET_H__
