#pragma once
#ifndef __MUZI_MCLIENTENDPOINT_H__
#define __MUZI_MCLIENTENDPOINT_H__
#include<string>
#include<boost/asio.hpp>
#include<boost/system/error_code.hpp>
#include"MNet/MNetBase.h"
namespace MUZI::NET
{
	class MClientEndPoint
	{
	public:
		struct MClientEndPointData;
	public:
		MClientEndPoint(String raw_ip_address, uint16_t port, int& ec);
		MClientEndPoint(const MClientEndPoint& endpoint);
		MClientEndPoint(MClientEndPoint&& endpoint);
		~MClientEndPoint();
	public:
		int createEndPoint();
		EndPoint* getEndPoint(int& error_code);
		EndPoint* getEndPoint(int& error_code) const;
	private:
		struct MClientEndPointData* m_data;
	};
}




#endif // !__MENDPOINT_H__
