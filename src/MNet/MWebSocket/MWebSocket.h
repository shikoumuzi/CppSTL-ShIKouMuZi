#pragma once
#ifndef __MUZI_MWEBSOCKET_H__
#define __MUZI_MWEBSOCKET_H__
#include"MNet/MNetBase.h"
#include<boost/beast.hpp>
#include<boost/beast/websocket.hpp>
#include<boost/beast/websocket/stream.hpp>
#include<memory>
#include<boost/uuid/uuid.hpp>
#include<boost/uuid/random_generator.hpp>
#include<boost/uuid/uuid_io.hpp>
#include<queue>
#include"MSTL/include/MSyncAnnularQueue.h"
#include"MSTL/include/MAtomicLock.h"
#include"MDesignModel/MSingleton/MSingleton.h"
#include<mutex>
namespace MUZI::net
{
	class MWebSocketServer : public singleton::MSingleton<MWebSocketServer>
	{
	public:
		class MWebSocketConnection : public std::enable_shared_from_this<class MWebSocketConnection>
		{
		public:
			using WebSocketStream = boost::beast::websocket::stream < boost::beast::tcp_stream>;
		public:
			MWebSocketConnection(MWebSocketServer* parent);
			MWebSocketConnection(MWebSocketServer* parent, IOContext& io_context);
			MWebSocketConnection(const MWebSocketConnection&) = delete;

		public:
			static String createUUID();
		public:
			const String& getUUID();
			TCPSocket& getSocket();
		public:
			void start();
		public:
			void accept();
			void send(const void* data, size_t data_size);
			void send(const String&);
		public:
			CLASSDATA(class MWebSocketConnection)* m_data;
		};
	public:
		//using WebSocketRequest = boost::beast::http::request;
	private:
		MWebSocketServer();
	public:
		MWebSocketServer(const MWebSocketServer&) = delete;
	public:
		void operator=(const MWebSocketServer&) = delete;
	public:
		void accept();

	public:
		void addConnection();
		void eraseConnection();

	public:
		class MWebSocketServerData* m_data;
	};
}

#endif // !__MUZI_MWEBSOCKET_H__
