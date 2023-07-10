#include "MServerEndPoint.h"
#include"../MError.h"
namespace MUZI::NET
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
	MServerEndPoint::MServerEndPoint(MServerEndPoint&& endpoint):m_data(endpoint.m_data)
	{
		endpoint.m_data = nullptr;
	}
	MServerEndPoint::~MServerEndPoint()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	EndPoint* MServerEndPoint::getEndPoint(int& error_code)
	{
		// TODO: 在此处插入 return 语句
		if (this->m_data != nullptr)
		{
			return &this->m_data->endpoint;
		}
		else
		{
			error_code = MERROR::OBJECT_IS_NULL;
			return nullptr;
		}
	}

	EndPoint* MServerEndPoint::getEndPoint(int& error_code) const
	{
		// TODO: 在此处插入 return 语句
		if (this->m_data != nullptr)
		{
			return &this->m_data->endpoint;
		}
		else
		{
			error_code = MERROR::OBJECT_IS_NULL;
			return nullptr;
		}
	}

}
