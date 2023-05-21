#ifndef __MUZI_MFTP_H__
#define __MUZI_MFTP_H__
#include<stdint.h>
#include<boost/asio/ip/udp.hpp>
#include<boost/asio/ip/tcp.hpp>
#include<boost/asio/io_context.hpp>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include<time.h>
class MFTP
{
private:
	enum ProtoType
	{
		IDENTIFICATION = 1,
		DATA,
		END

	};
	struct Proto
	{
		struct BaseMessage
		{
			uint64_t id; // 包序号，用来检查
			uint64_t size; // 包大小，用以检查包完整性
			unsigned char type;// 包类型
			uint64_t passwd;// 验证信息
		};
		struct Identification:public BaseMessage
		{
			uint16_t client_ip4_addr;
			uint16_t server_ip4_addr;
			unsigned int client_port;
			unsigned int server_port;
			time_t time;
		};
		struct Data :public BaseMessage
		{
			time_t time;
			char data[0];
		};
		struct Termination :public BaseMessage
		{
		};
	};
public:
	MFTP()
	{
		
	}
	~MFTP()
	{

	}
public:

private:
	
};



#endif // !__MUZI_MFTP_H__

