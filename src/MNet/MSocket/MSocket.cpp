#include "MSocket.h"
#include"MLog/MLog.h"
#include"MBase/MError.h"
namespace MUZI::NET
{

	struct MSocket::MSocketData
	{
		IOContext io_context;
		Protocol protocol;
		struct ServerSocketData
		{
			TCPAcceptor acceptor;
		};
		struct ClientSocketData
		{
			TCPSocket socket;
		};
		union SocketData
		{
			ServerSocketData server;
			ClientSocketData client;
		}data;
		bool isServer;
		union LocalEndPoint
		{
			MServerEndPoint server_endpoint;
			MClientEndPoint client_endpoint;
		}local_endpoint;
	};

	MSocket::MSocket() : m_data(static_cast<MSocketData*>(operator new(sizeof(MSocketData))))
	{
		new(&(this->m_data->io_context)) IOContext;
	}

	MSocket::MSocket(const MServerEndPoint& endpoint) :MSocket()
	{
		this->m_data->isServer = true;
		new(&(this->m_data->protocol)) Protocol(Protocol::v4());// 初始化协议
		new(&(this->m_data->data.server)) MSocketData::ServerSocketData({std::move(TCPAcceptor(this->m_data->io_context))});// 初始化acceptor
		new(&(this->m_data->local_endpoint.server_endpoint)) MServerEndPoint(endpoint);// 初始化endpoint
	}

	MSocket::MSocket(const MClientEndPoint& endpoint)
	{
		this->m_data->isServer = false;
		int ec = 0;
		EndPoint* ep = endpoint.getEndPoint(ec);
		new(&(this->m_data->protocol)) Protocol(Protocol::v4());
		new(&(this->m_data->data.client)) MSocketData::ClientSocketData({ std::move(TCPSocket(this->m_data->io_context)) });// 初始化socket
		new(&(this->m_data->local_endpoint.client_endpoint)) MClientEndPoint(endpoint);
	}

	MSocket::~MSocket()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			m_data = nullptr;
		}
	}

	int MSocket::bind()
	{
		if (this->m_data->isServer)
		{
			int ec = 0;
			EC error_code;
			EndPoint* endpoint = this->m_data->local_endpoint.server_endpoint.getEndPoint(ec);
			if (endpoint == nullptr)
			{
				MLog::w("MSocket::bind", "endpoint is not construct, Error Code is %d", ec);
				return ec;
			}
			this->m_data->data.server.acceptor.bind(*endpoint, error_code);
			if (error_code.value() != 0)
			{
				MLog::w("MSocket::bind", "endpoint is not construct, Error Code is %d, Error Message is %s", MERROR::BIND_ERROR, error_code.message().c_str());
				return MERROR::BIND_ERROR;
			}
		}
		else
		{
			return MERROR::OBJECT_IS_NO_SERVER;
		}
		return 0;
	}

	int MSocket::connect(const MServerEndPoint& endpoint)
	{
		EC error_code;
		int ec = 0;
		EndPoint* ep = endpoint.getEndPoint(ec);
		if (ep == nullptr)
		{
			MLog::w("MSocket::bind", "endpoint is not construct, Error Code is %d", MERROR::ENDPOINT_IS_NO_CREATED);
			return ec;
		}
		this->m_data->data.client.socket.connect(*ep, error_code);
		if (error_code.value() != 0)
		{
			MLog::w("MSocket::bind", "endpoint is not construct, Error Code is %d, Error Message is %s", MERROR::CONNECT_ERROR, error_code.message().c_str());
			return MERROR::BIND_ERROR;
		}

		return 0;
	}

}
