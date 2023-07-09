#include "MSocket.h"
#include"MLog/MLog.h"
#include"MBase/MError.h"
#include"boost/algorithm/string.hpp"
namespace MUZI::NET
{

	struct MSocket::MSocketData
	{
	public:
		struct ServerSocketData
		{
			TCPAcceptor acceptor;
			TCPSocket socket;
		};
		struct ClientSocketData
		{
			TCPSocket socket;
		};
		union SocketData
		{
			ServerSocketData server;
			ClientSocketData client;
		};
		union LocalEndPoint
		{
			MServerEndPoint server_endpoint;
			MClientEndPoint client_endpoint;
		};

	public:
		bool isServer;
		IOContext io_context;
		Protocol protocol;
		union SocketData data;
		union LocalEndPoint local_endpoint;
	};

	MSocket::MSocket() : m_data(static_cast<MSocketData*>(operator new(sizeof(MSocketData))))
	{
		new(&(this->m_data->io_context)) IOContext;
	}

	MSocket::MSocket(const MServerEndPoint& endpoint) :MSocket()
	{
		this->m_data->isServer = true;
		new(&(this->m_data->protocol)) Protocol(Protocol::v4());// 初始化协议
		new(&(this->m_data->data.server)) MSocketData::ServerSocketData({std::move(TCPAcceptor(this->m_data->io_context)), std::move(TCPSocket(this->m_data->io_context))});// 初始化acceptor
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
			this->m_data->io_context.~io_context();
			if (this->m_data->isServer)
			{
				this->m_data->data.server.acceptor.~basic_socket_acceptor();
				this->m_data->data.client.socket.~basic_stream_socket();
				this->m_data->local_endpoint.server_endpoint.~MServerEndPoint();
			}
			else
			{
				this->m_data->data.client.socket.~basic_stream_socket();
				this->m_data->local_endpoint.client_endpoint.~MClientEndPoint();
			}
			operator delete(this->m_data);
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
				MLog::w("MSocket::bind", "bind is failed, Error Code is %d, Error Message is %s", MERROR::BIND_ERROR, error_code.message().c_str());
				return MERROR::BIND_ERROR;
			}
		}
		else
		{
			return MERROR::OBJECT_IS_NO_SERVER;
		}
		return 0;
	}

	int MSocket::listen()
	{
		EC ec;
		this->m_data->data.server.acceptor.listen(this->back_log);
		if (ec.value() != 0)
		{
			MLog::w("MSocket::accept", "listen is failed, Error Code is %d, Error Message is %s", MERROR::LISTEN_ERROR, ec.message());
			return  MERROR::LISTEN_ERROR;
		}
		return 0;
	}

	int MSocket::accept()
	{
		EC ec;
		this->m_data->data.server.acceptor.accept(this->m_data->data.server.socket, ec);
		if (ec.value() != 0)
		{
			MLog::w("MSocket::accept", "accept is failed, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message());
			return  MERROR::ACCEPT_ERROR;
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
			MLog::w("MSocket::bind", "connet is failed, Error Code is %d, Error Message is %s", MERROR::CONNECT_ERROR, error_code.message().c_str());
			return MERROR::BIND_ERROR;
		}

		return 0;
	}

	int MSocket::connect(const String& host, Port port)
	{
		HostQuery resolver_query(host, std::to_string(port), HostQuery::numeric_service);
		HostResolver resolver(this->m_data->io_context);
		EC ec;
		// 这里采用查询器和解析器进行，由于返回结果可能不止一个，所以需要用迭代器进行轮询
		HostResolver::iterator it = resolver.resolve(resolver_query);
		boost::asio::connect(this->m_data->data.client.socket, it,ec);
		if (ec.value() != 0)
		{
			MLog::w("MSocket::connect", "connet is failed, Error Code is %d, Error Message is %s", MERROR::CONNECT_ERROR, ec.message().c_str());
		}
		return 0;
	}
	int MSocket::write(String& data, int& error_code)
	{
		this->write((char*)(data.c_str()), data.size(), error_code);
	}

	int MSocket::write(void* data, uint64_t data_size, int& error_code)
	{
		uint64_t total_bytes = 0;
		error_code = 0;
		if (this->m_data->isServer)
		{
			while (total_bytes != data_size)
			{
				// 通过调用write_some来向网络写入数据，每次从偏移量开始， 类似Linux 的send api
				this->m_data->data.server.
					socket.write_some(boost::asio::buffer(static_cast<char*>(data) + total_bytes, data_size - total_bytes));

			}
		}
		else
		{
			while (total_bytes != data_size)
			{
				this->m_data->data.client.
					socket.write_some(boost::asio::buffer(static_cast<char*>(data) + total_bytes, data_size - total_bytes));
			}
		}
		return 0;
	}

	int MSocket::read()
	{
		return 0;
	}


}
