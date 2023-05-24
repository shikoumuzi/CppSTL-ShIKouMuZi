#ifndef __MUZI_MFTP_H__
#define __MUZI_MFTP_H__
#include<stdint.h>
#include<boost/asio/ip/udp.hpp>
#include<boost/asio/ip/tcp.hpp>
#include<boost/asio/io_context.hpp>
#include<string>
#include<boost/filesystem.hpp>
#include<boost/algorithm/string.hpp>
#include<time.h>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include"MFTPProto.h"
namespace MUZI
{
	class MFTP
	{
	public:
		enum ROLE_ID
		{
			UNKOWN = 0,
			CLIENT,
			SERVER
		};
		enum ROLE_MODE
		{
			One2Mulit = 0,
			One2One,
		};
	private:
		enum ProtoType;
		enum ProtoID;
		using ROLE = size_t;
	public:
		MFTP();// 默认构造函数，会先用组播方式寻找对方并连接
		MFTP(ROLE role, uint16_t client_ip4_addr, unsigned int client_port, uint16_t server_ip4_addr, unsigned int server_port);// 知道对方地址
		MFTP(const MFTP& that) = delete;
		MFTP(MFTP&&) noexcept;
	public:
		int searchEachOther();
		int connect();
		int package();
		int disconnect();
	private:
		Identification* createIdentification();
		Data* createData();
		Termination* createTermination();
	private:
		Identification identification;
		ROLE role;
		char* root_file_directory;
	};


}

#endif // !__MUZI_MFTP_H__

