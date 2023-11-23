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
		MHttpClient(IOContext& io_context, const String& server, const String path);

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
	};
}

#endif // !__MUZI_MHTTPCLIENT_H__