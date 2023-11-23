#pragma once
#ifndef __MUZI_MHTTPCLIENT_H__
#define __MUZI_MHTTPCLIENT_H__
#include"MNet/MNetBase.h"
#include<iostream>
namespace MUZI::net::http
{
	class MHttpClient
	{
	public:
		MHttpClient(const String& server, const String path);
		MHttpClient(IOContext& io_context, const String& server, const String path);
		MHttpClient(const MHttpClient&) = delete;
		MHttpClient(MHttpClient&& client);
		~MHttpClient();
	public:
		void operator=(const MHttpClient&) = delete;
		void operator=(MHttpClient&& client);
	public:
		void handleResolver(const EC& ec, const TCPResolver::results_type& endpoints);
		void handleConnect(const EC& ec, boost::asio::ip::tcp::endpoint endpoint);
		void handleWriteRequest(const EC& ec, size_t byte);
		void handleRead(const EC& ec, size_t byte);
	public:
		TCPSocket& getSocket();
		Request& getRequest();
		Reponse& getReponse();
		IOContext& getIOContext();
	private:
		TCPResolver m_resolver;
		IOContext* m_io_context;
		TCPSocket m_socket;
		Request m_request;
		Reponse m_reponse;
		bool m_new_io_context_flag;
	};
}

#endif // !__MUZI_MHTTPCLIENT_H__
