#ifndef __MUZI_MFTPPROTO_H__
#define __MUZI_MFTPPROTO_H__
#include<stdint.h>
#include<time.h>
namespace MUZI
{
	struct BaseMessage
	{
		uint64_t id; // ����ţ��������
		uint64_t size; // ����С�����Լ���������
		unsigned char type;// ������
		uint64_t passwd;// ��֤��Ϣ
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