#pragma once 
#ifndef __MUZI_MSOCKET_H__
#define __MUZI_MSOCKET_H__
#include"../MNetBase.h"
#include"MNet/MEndPoint/MServerEndPoint.h"
#include"MNet/MEndPoint/MClientEndPoint.h"

namespace MUZI::NET
{
	class MSocket
	{
	public:
		static int back_log;
	public:
		struct MSocketData;
	public:
		MSocket();
		MSocket(const MServerEndPoint& endpoint);
		MSocket(const MClientEndPoint& endpoint);
		~MSocket();
	public:// server
		int bind();
		int listen();
		int accept();
	public:// client
		int connect(const MServerEndPoint& endpoint);
		/// @brief 
		/// @param host an str witch is decribe net
		/// @return return 0 if success, other is error_code
		int connect(const String& host, Port port);
		
	private:
		struct MSocketData* m_data;
	};


	int MSocket::back_log = 30;

}

#endif // !__MUZI_MSOCKET_H__
