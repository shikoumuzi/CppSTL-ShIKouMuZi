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
	enum ProtoType
	{
		IDENTIFICATION = 1,
		DATA,
		END

	};
	enum ProtoID
	{
		MFTP_BASE_MESSAGE = -1,
		MFTP_IDENTIFICATION,
	};
	using ROLE = size_t;
	union Proto
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
	};
public:
	// Ĭ�Ϲ��캯�����������鲥��ʽѰ�ҶԷ�������
	MFTP() 
		:identification({ Proto::BaseMessage(ProtoID::MFTP_BASE_MESSAGE, sizeof(Proto::Identification), ProtoType::IDENTIFICATION, 0), 0, 0, 0, 0, 0 }),
		role(ROLE_ID::UNKOWN){}
	// ֪���Է���ַ
	MFTP(ROLE role , uint16_t client_ip4_addr, unsigned int client_port, uint16_t server_ip4_addr, unsigned int server_port)
		:identification({ Proto::BaseMessage(ProtoID::MFTP_BASE_MESSAGE, sizeof(Proto::Identification), ProtoType::IDENTIFICATION, 0), 
			client_ip4_addr, server_ip4_addr, client_port, server_port, 0 }){}
public:
	int searchEachOther()
	{
		if (this->role == UNKOWN)
		{

		}
		return 0;
	}
	int connect()
	{
		Proto::Identification identification;
		memcpy(&identification, &this->identification.base_message, sizeof(Proto::BaseMessage));
	}
	// ���ÿ���Ȩ˼�룬��֤Data��������Ȩ��FTP����
	int package()
	{

	}
	int disconnect()
	{

	}
private:
	Proto::Identification createIdentification()
	{

	}
	Proto::Data createData()
	{

	}
	Proto::Termination createTermination()
	{

	};
private:
	Proto::Identification identification;
	ROLE role;
};



#endif // !__MUZI_MFTP_H__

