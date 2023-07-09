#include "MServerEndPoint.h"
#include"../MError.h"
namespace MUZI::NET
{
	struct MServerEndPoint::MServerEndPointData
	{
		Address server_address;
		uint16_t server_port;
		EndPoint endpoint;

	};

	MServerEndPoint::MServerEndPoint(uint16_t port) :m_data(new MServerEndPointData())
	{
		EC error_code;
		this->m_data->server_address = AddressSeccion::any();
		this->m_data->server_port = port;
		
	}
	MServerEndPoint::MServerEndPoint(const MServerEndPoint& endpoint)
	{
		if (this->m_data == nullptr)
		{
			this->m_data = new MServerEndPointData;
		}
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
	int MServerEndPoint::createEndPoint()
	{
		if (this->m_data != nullptr)
		{
			this->m_data->endpoint.address(this->m_data->server_address);
			this->m_data->endpoint.port(this->m_data->server_port);
			return 0;
		}
		else
		{
			return MERROR::OBJECT_IS_NULL;
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
		return this->getEndPoint(error_code);
	}

}
