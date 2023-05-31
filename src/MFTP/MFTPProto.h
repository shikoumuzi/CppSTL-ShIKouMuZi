#ifndef __MUZI_MFTPPROTO_H__
#define __MUZI_MFTPPROTO_H__
#include<stdint.h>
#include<time.h>
namespace MUZI
{
	struct BaseMessage
	{
		uint64_t id; // 包序号，用来检查
		uint64_t size; // 包大小，用以检查包完整性
		unsigned char type;// 包类型
		uint64_t passwd;// 验证信息
	};
	struct Identification
	{
		struct BaseMessage base_message;
		uint16_t client_ip4_addr;
		uint16_t server_ip4_addr;
		unsigned int client_port;
		unsigned int server_port;
		time_t time;
	};
	struct Data
	{
		struct BaseMessage base_message;
		char data[0];
	};
	struct Termination
	{
		struct BaseMessage base_message;
	};
	
}
#endif // !__MUZI_MFTPPROTO_H__