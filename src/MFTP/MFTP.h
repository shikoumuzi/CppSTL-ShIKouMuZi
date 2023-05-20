#ifndef __MUZI_MFTP_H__
#define __MUZI_MFTP_H__
#include<stdint.h>
#include<boost/asio/ip/udp.hpp>
#include<boost/asio/ip/tcp.hpp>
#include<boost/asio/io_context.hpp>

class MFTP
{
private:
	struct Proto
	{
		uint64_t id; // ����ţ��������
		uint64_t size; // ����С�����Լ���������
		char data[0];
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

