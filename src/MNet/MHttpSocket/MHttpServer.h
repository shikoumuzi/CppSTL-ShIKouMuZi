#pragma once
#ifndef __MUZI_M_HTTPSERVER_H__
#define __MUZI_M_HTTPSERVER_H__
#include<boost/beast.hpp>
#include<boost/asio.hpp>
#include<ctime>
#include<chrono>
#include<iostream>

namespace MUZI::net::http
{
	class MHttpServer
	{
	public:
		MHttpServer();
	public:
		class HttpServerData* m_data;
	};
}

#endif // !__MUZI_M_HTTPSERVER_H__
