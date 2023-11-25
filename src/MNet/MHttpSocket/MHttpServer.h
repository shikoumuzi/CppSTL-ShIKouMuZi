#pragma once
#ifndef __MUZI_M_HTTPSERVER_H__
#define __MUZI_M_HTTPSERVER_H__
#include<boost/beast.hpp>
#include<boost/asio.hpp>
#include<ctime>
#include<chrono>
#include<iostream>
#include<memory>
#include"MNet/MNetBase.h"
#include<boost/beast.hpp>
#define __MUZI_MHTTPSERVER_BUFFER_SIZE__ 8192
#define __MUZI_MHTTPSERVER_TIMEOUT_VALUE__ 60

namespace MUZI::net::http
{
	class MHttpServer
	{
	public:
		class HttpConnection: public std::enable_shared_from_this<HttpConnection>
		{
		public:
			HttpConnection(TCPSocket& socket, size_t buffer_size = __MUZI_MHTTPSERVER_BUFFER_SIZE__, size_t time_out = __MUZI_MHTTPSERVER_TIMEOUT_VALUE__);
			~HttpConnection();
		public:
			void start();
		public:
			void readRequest();
			void checkDeadline();
		public:
			class HttpConnectionData* m_data;
		};
	public:
		MHttpServer();
	public:
		class HttpServerData* m_data;
	};
}

#endif // !__MUZI_M_HTTPSERVER_H__
