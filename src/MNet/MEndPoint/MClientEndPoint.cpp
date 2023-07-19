#include "MClientEndPoint.h"
#include"../MError.h"
namespace MUZI::NET
{
	struct MClientEndPoint::MClientEndPointData
	{
		Address server_address;
		Port server_port;
		EndPoint endpoint;
	};


	MClientEndPoint::MClientEndPoint(String raw_ip_address, Port port, int& ec) :m_data(new MClientEndPointData())
	{
		EC error_code;
		this->m_data->server_address = Address::from_string(raw_ip_address, error_code);
		
		if (error_code.value() != 0)
		{
			ec = MERROR::IPADDRESS_FORMAT_CANNOT_ANALIZE;
			fprintf(stderr, "Failed to parse the IP address, Error Code = %d, Message is %s", MERROR::IPADDRESS_FORMAT_CANNOT_ANALIZE, error_code.message().c_str());
			this->~MClientEndPoint();
			return;
		}
		this->m_data->endpoint.address(this->m_data->server_address);
		this->m_data->endpoint.port(port);

		ec = 0;
	}
	MClientEndPoint::MClientEndPoint(const MClientEndPoint& endpoint)
	{
		if (this->m_data == nullptr)
		{
			this->m_data = new MClientEndPointData;
		}
		this->m_data->server_address = endpoint.m_data->server_address;
		this->m_data->server_port = endpoint.m_data->server_port;
		this->m_data->endpoint = endpoint.m_data->endpoint;
	}
	MClientEndPoint::MClientEndPoint(MClientEndPoint&& endpoint):m_data(endpoint.m_data)
	{
		endpoint.m_data = nullptr;
	}
	MClientEndPoint::~MClientEndPoint()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	EndPoint* MClientEndPoint::getEndPoint()
	{

		return &this->m_data->endpoint;

	}

	EndPoint* MClientEndPoint::getEndPoint() const
	{
		return &this->m_data->endpoint;
	}

}
