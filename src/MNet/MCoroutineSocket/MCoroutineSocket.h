#pragma once
#ifndef __MUZI_MCOROUTINESOCKET_H__
#define __MUZI_MCOROUTINESOCKET_H__
#include<boost/asio.hpp>
#include"MNet/MNetBase.h"
#include<coroutine>

namespace MUZI::net::coroutine
{
	class MCoroutineSocket
	{
	public:
		template<typename T>
		using Await = boost::asio::awaitable<T>;
	public:
		MCoroutineSocket();
		MCoroutineSocket(const IOContext& io_context);
	public:
		Await<void> listener();
	public:
		class MCoroutineSocketData* m_data;
	};
}

#endif // !__MUZI_MCOROUTINESOCKET_H__
