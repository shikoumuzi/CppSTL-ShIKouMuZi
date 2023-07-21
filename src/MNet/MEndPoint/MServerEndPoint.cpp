#include "MServerEndPoint.h"
#include"../MError.h"
namespace MUZI::net
{
	struct MServerEndPoint::MServerEndPointData
	{
		Address server_address;
		Port server_port;
		EndPoint endpoint;

	};

	MServerEndPoint::MServerEndPoint(Port port) :m_data(new MServerEndPointData())
	{
		this->m_data->server_address = AddressSeccion::any();
		this->m_data->server_port = port;
		this->m_data->endpoint.address(this->m_data->server_address);
		this->m_data->endpoint.port(port);
	}
	MServerEndPoint::MServerEndPoint(const MServerEndPoint& endpoint) :m_data(new MServerEndPointData())
	{
		this->m_data->server_address = endpoint.m_data->server_address;
		this->m_data->server_port = endpoint.m_data->server_port;
		this->m_data->endpoint = endpoint.m_data->endpoint;
	}
	MServerEndPoint::~MServerEndPoint()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	EndPoint* MServerEndPoint::getEndPoint()
	{
		return &this->m_data->endpoint;
	}

	EndPoint* MServerEndPoint::getEndPoint() const
	{
		return &this->m_data->endpoint;
	}

}
