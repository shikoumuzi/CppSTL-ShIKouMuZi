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
		struct MSocketData;
	public:
		MSocket();
		MSocket(const MServerEndPoint& endpoint);
		MSocket(const MClientEndPoint& endpoint);
		~MSocket();
	public:// server
		int bind();
	public:// client
		int connect(const MServerEndPoint& endpoint);
		int connect(const String& dns);

	private:
		struct MSocketData* m_data;
	};


}

#endif // !__MUZI_MSOCKET_H__
