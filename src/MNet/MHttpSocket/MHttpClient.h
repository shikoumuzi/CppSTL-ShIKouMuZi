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
		MHttpClient(IOContext& io_context, const String& server, const String path) ;
		MHttpClient(const MHttpClient&) = delete;
		MHttpClient(MHttpClient&& client) noexcept;
		~MHttpClient();
	public:
		void operator=(const MHttpClient&) = delete;
		void operator=(MHttpClient&& client) noexcept;
	public:
		void handleResolver(const EC& ec, const TCPResolver::results_type& endpoints);
		void handleConnect(const EC& ec, boost::asio::ip::tcp::endpoint endpoint);
		void handleWriteRequest(const EC& ec, size_t byte); // ����ͷ����������
		void handleReadStatusLine(const EC& ec, size_t byte); // ������Ӧ��ͷ����Ϣ
		void handleReadHeaders(const EC& ec, size_t byte); // ����ʣ�����е�ͷ����Ϣ
		void handleReadContent(const EC& ec, size_t byte); // ��������
	public:
		TCPSocket& getSocket();
		RequestBuffer& getRequest();
		ReponseBuffer& getResponse();
		IOContext& getIOContext();
	public:
		void run();
	private:
		TCPResolver m_resolver;
		IOContext* m_io_context;
		TCPSocket m_socket;
		RequestBuffer m_request;
		ReponseBuffer m_response;
		bool m_new_io_context_flag;
	};
}

#endif // !__MUZI_MHTTPCLIENT_H__
