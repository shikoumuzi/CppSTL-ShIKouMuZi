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
#include<boost/filesystem.hpp>
#include<iostream>
#include<fstream>
#include<boost/filesystem/string_file.hpp>

#define __MUZI_MHTTPSERVER_BUFFER_SIZE__ 8192
#define __MUZI_MHTTPSERVER_TIMEOUT_VALUE__ 60

namespace MUZI::net::http
{
	class MHttpServer
	{
	public:
		friend class HttpConnection;
	public:
		using FilePath = boost::filesystem::path;

	public:
		class HttpConnection : public std::enable_shared_from_this<HttpConnection>
		{
		public:
			using HttpVerb = boost::beast::http::verb;
			using HttpStatus = boost::beast::http::status;
			using HttpField = boost::beast::http::field;
			static int DirectoryMode;
		public:
			HttpConnection(class MHttpServer* parent, TCPSocket& socket, size_t buffer_size = __MUZI_MHTTPSERVER_BUFFER_SIZE__, size_t time_out = __MUZI_MHTTPSERVER_TIMEOUT_VALUE__);
			~HttpConnection();
		public:
			void start();
		public:
			void readRequest();
			void checkDeadline();
			void processRequest();
			void createGetResponse();
			void createPostResponse();
			void writeResponse();
		public:
			bool registerPath(String& target, const FilePath& file_path);
			bool resisterPath(String& target, const FilePath& dir_path, int directory_mode);
			void registerPaths(const FilePath& dir_path, int deepth = 0);
		public:
			class HttpConnectionData* m_data;
		};
	public:
		MHttpServer();
	public:
		void accept(TCPAcceptor& acceptor, TCPSocket& socket);
	public:
		class HttpServerData* m_data;
	};
}

#endif // !__MUZI_M_HTTPSERVER_H__
