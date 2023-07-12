#pragma once 
#ifndef __MUZI_MSOCKET_H__
#define __MUZI_MSOCKET_H__
#include"../MNetBase.h"
#include"MNet/MEndPoint/MServerEndPoint.h"
#include"MNet/MEndPoint/MClientEndPoint.h"

namespace MUZI::NET::SYNC
{
	class MSyncSocket
	{
	public:
		static int back_log;
	public:
		struct MSyncSocketData;
	public:
		MSyncSocket();
		MSyncSocket(const MServerEndPoint& endpoint);
		MSyncSocket(const MClientEndPoint& endpoint);
		~MSyncSocket();
	public:// server
		int bind();
		int listen();
		NetIOAdapt accept(int& error_code);
	public:// client
		int connect(const NetIOAdapt& adapt, const MServerEndPoint& endpoint);
		/// @brief 
		/// @param host an str witch is domain name
		/// @return return 0 if success, other is error_code
		int connect(const NetIOAdapt& adapt, const String& host, Port port);
		
	public:// io
		int write(const NetIOAdapt& adapt, const String& data);
		int write(const NetIOAdapt& adapt, void* data, uint64_t data_size);
		int read(const NetIOAdapt& adapt, void* buff, uint64_t requiredsize, bool immediate_request_mode = false);


	private:
		struct MSyncSocketData* m_data;
	};




}

#endif // !__MUZI_MSOCKET_H__
