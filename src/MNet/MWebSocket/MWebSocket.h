#pragma once
#ifndef __MUZI_MWEBSOCKET_H__
#define __MUZI_MWEBSOCKET_H__
#include"MNet/MNetBase.h"
namespace MUZI::net
{
	class MWebSocketServer
	{
	public:
		MWebSocketServer();
	public:
		void accept();
	public:
		class MWebSocketServerData* m_data;
	};
}

#endif // !__MUZI_MWEBSOCKET_H__
