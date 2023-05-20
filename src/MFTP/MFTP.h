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
		uint64_t id; // 包序号，用来检查
		uint64_t size; // 包大小，用以检查包完整性
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

