#pragma once
#ifndef __MUZI_MSERVERENDPOINT_H__
#define __MUZI_MSERVERENDPOINT_H__

#include<string>
#include<boost/asio.hpp>
#include<boost/system/error_code.hpp>
#include"MNet/MNetBase.h"
#include"MEndPoint.h"
namespace MUZI::net
{
	class MServerEndPoint :public IMEndPoint
	{
	public:
		struct MServerEndPointData;
	public:
		MServerEndPoint(Port port);
		MServerEndPoint(const MServerEndPoint& endpoint);
		MServerEndPoint(MServerEndPoint&& endpoint) = delete;
		~MServerEndPoint();
	public:
		TCPEndPoint* getEndPoint() override;
		TCPEndPoint* getEndPoint() const;
	private:
		struct MServerEndPointData* m_data;
	};
}

#endif // !__MUZI_MSERVERENDPOINT_H__
