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
		template<typename T>
		using Awaitable = boost::asio::awaitable<T>;
		using iterator = std::map<String, MCoroSessionPack>::iterator;
	public:
		MCoroutineSocket();
		MCoroutineSocket(IOContext& io_context);
	public:
		IOContext& getIOContext();
	public:
		MCoroSessionPack& getSession(const String& uuid);
		Map<String, MCoroSessionPack>& getSessions();
	public:
		void run();
	public:
		iterator begin();
		iterator end();
		void erase(String UUID);
		iterator erase(iterator& it);
	public:
		class MCoroutineSocketData* m_data;
	};
}

#endif // !__MUZI_MCOROUTINESOCKET_H__
 