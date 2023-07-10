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
		};
		struct ClientSocketData
		{
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

	int MSocket::back_log = 30;

	MSocket::MSocket() : m_data(static_cast<MSocketData*>(operator new(sizeof(MSocketData))))
	{
		new(&(this->m_data->io_context)) IOContext;
	}

	MSocket::MSocket(const MServerEndPoint& endpoint) :MSocket()
	{
		int ec = 0;
		this->m_data->isServer = true;
		new(&(this->m_data->protocol)) Protocol(Protocol::v4());// 初始化协议
		new(&(this->m_data->data.server)) \
			MSocketData::ServerSocketData({ std::move(TCPAcceptor(this->m_data->io_context, endpoint.getEndPoint(ec)->protocol()))});// 初始化acceptor
		new(&(this->m_data->local_endpoint.server_endpoint)) MServerEndPoint(endpoint);// 初始化endpoint
		//this->m_data->data.server.acceptor
	}

	MSocket::MSocket(const MClientEndPoint& endpoint)
	{
		this->m_data->isServer = false;
		int ec = 0;
		EndPoint* ep = endpoint.getEndPoint(ec);
		new(&(this->m_data->protocol)) Protocol(Protocol::v4());
		new(&(this->m_data->data.client)) MSocketData::ClientSocketData();// 初始化socket
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
				this->m_data->local_endpoint.server_endpoint.~MServerEndPoint();
			}
			else
			{
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
				//MLog::w("MSocket::bind", "endpoint is not construct, Error Code is %d", ec);
				return ec;
			}
			this->m_data->data.server.acceptor.bind(*endpoint, error_code);
			if (error_code.value() != 0)
			{
				std::cerr << error_code.message();
				char* data = error_code.message().data();

				//MLog::w("MSocket::bind", "bind is failed, Error Code is %d, Error Message is %s", MERROR::BIND_ERROR, data);
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
		if (this->m_data->isServer)
		{
			EC ec;
			this->m_data->data.server.acceptor.listen(this->back_log, ec);
			if (ec.value() != 0)
			{
				std::cerr << ec.message() << std::endl;
				//MLog::w("MSocket::accept", "listen is failed, Error Code is %d, Error Message is %s", MERROR::LISTEN_ERROR, ec.message());
				return  MERROR::LISTEN_ERROR;
			}
			return 0;
		}
		else
		{
			return MERROR::OBJECT_IS_NO_SERVER;
		}
	}

	NetIOAdapt MSocket::accept(int& error_code)
	{
		if (this->m_data->isServer)
		{
			EC ec;
			NetIOAdapt socket(new TCPSocket(this->m_data->io_context));
			/*std::make_shared<TCPSocket>(std::move(TCPSocket(this->m_data->io_context, this->m_data->protocol)));*/
			this->m_data->data.server.acceptor.accept(*socket, ec);
			if (ec.value() != 0)
			{
				std::cerr << ec.message();
				/*MLog::w("MSocket::accept", "accept is failed, Error Code is %d, Error Message is %s", MERROR::ACCEPT_ERROR, ec.message().c_str());*/
				error_code = MERROR::ACCEPT_ERROR;
				return NetIOAdapt();
			}
			return socket;
		}
		else
		{
			error_code = MERROR::OBJECT_IS_NO_SERVER;
			return NetIOAdapt();
		}
	}

	int MSocket::connect(const NetIOAdapt& adapt, const MServerEndPoint& endpoint)
	{
		if (!this->m_data->isServer)
		{
			EC error_code;
			int ec = 0;
			const EndPoint* ep = endpoint.getEndPoint(ec);
			if (ep == nullptr)
			{
				MLog::w("MSocket::bind", "endpoint is not construct, Error Code is %d", MERROR::ENDPOINT_IS_NO_CREATED);
				return ec;
			}
			adapt->connect(*ep, error_code);
			if (error_code.value() != 0)
			{
				MLog::w("MSocket::bind", "connet is failed, Error Code is %d, Error Message is %s", MERROR::CONNECT_ERROR, error_code.message().c_str());
				return MERROR::BIND_ERROR;
			}

			return 0;
		}
		else
		{
			return MERROR::OBJECT_IS_NO_CLIENT;
		}
	}

	int MSocket::connect(const NetIOAdapt& adapt, const String& host, Port port)
	{
		if (!this->m_data->isServer)
		{
			HostQuery resolver_query(host, std::to_string(port), HostQuery::numeric_service);
			HostResolver resolver(this->m_data->io_context);
			EC ec;
			// 这里采用查询器和解析器进行，由于返回结果可能不止一个，所以需要用迭代器进行轮询
			HostResolver::iterator it = resolver.resolve(resolver_query);
			boost::asio::connect(*adapt, it, ec);
			if (ec.value() != 0)
			{
				MLog::w("MSocket::connect", "connet is failed, Error Code is %d, Error Message is %s", MERROR::CONNECT_ERROR, ec.message().c_str());
			}
			return 0;
		}
		else
		{
			return MERROR::OBJECT_IS_NO_CLIENT;
		}
	}
	int MSocket::write(const NetIOAdapt& adapt, String& data)
	{
		return this->write(adapt, (char*)(data.c_str()), data.size());
	}

	int MSocket::write(const NetIOAdapt& adapt, void* data, uint64_t data_size)
	{
		int64_t total_bytes = 0;
		EC ec;
		if (this->m_data->isServer)
		{
			while (total_bytes != data_size)
			{
				// 通过调用write_some来向网络写入数据，每次从偏移量开始， 类似Linux 的send api
				total_bytes += adapt->write_some(boost::asio::buffer(static_cast<char*>(data) + total_bytes, data_size - total_bytes), ec);
				if (ec.value() != 0)
				{
					MLog::w("MSocket::write", "socket have system error, Error Code is %d, Error Message is %d", MERROR::SOCKET_SYSTEM_ERROR, ec.message());
					return MERROR::SOCKET_SYSTEM_ERROR;
				}
			}
		}
		else
		{
			while (total_bytes != data_size)
			{
				total_bytes += adapt->write_some(boost::asio::buffer(static_cast<char*>(data) + total_bytes, data_size - total_bytes), ec);
				if (ec.value() != 0)
				{
					MLog::w("MSocket::write", "socket have system error, Error Code is %d, Error Message is %d", MERROR::SOCKET_SYSTEM_ERROR, ec.message());
					return MERROR::SOCKET_SYSTEM_ERROR;
				}
			}
		}
		return 0;
	}

	int MSocket::read(const NetIOAdapt& adapt, void* buff, uint64_t requiredsize, bool immediate_request_mode)
	{
		int64_t total_bytes = 0;
		EC ec;
		if (this->m_data->isServer)
		{
			if(immediate_request_mode)
			{
				// 通过调用read_some来向网络写入数据，每次从偏移量开始， 类似Linux 的receive api
				total_bytes += adapt->read_some(boost::asio::buffer(static_cast<char*>(buff) + total_bytes, requiredsize - total_bytes), ec);
				if (ec == boost::asio::error::eof)
				{
					return 0;
				}
				if (ec.value() != 0)
				{
					MLog::w("MSocket::read", "socket have system error, Error Code is %d, Error Message is %d", MERROR::SOCKET_SYSTEM_ERROR, ec.message());
					return MERROR::SOCKET_SYSTEM_ERROR;
				}
			}
			else
			{
				while (total_bytes != requiredsize)
				{
					// 通过调用read_some来向网络写入数据，每次从偏移量开始， 类似Linux 的receive api
					total_bytes += adapt->read_some(boost::asio::buffer(static_cast<char*>(buff) + total_bytes, requiredsize - total_bytes), ec);
					if (ec == boost::asio::error::eof)
					{
						return 0;
					}
					if (ec.value() != 0)
					{
						MLog::w("MSocket::read", "socket have system error, Error Code is %d, Error Message is %d", MERROR::SOCKET_SYSTEM_ERROR, ec.message());
						return MERROR::SOCKET_SYSTEM_ERROR;
					}
				}
			}
		}
		else
		{
			while (total_bytes != requiredsize)
			{
				adapt->read_some(boost::asio::buffer(static_cast<char*>(buff) + total_bytes, requiredsize - total_bytes), ec);
				if (ec.value() != 0)
				{
					MLog::w("MSocket::read", "socket have system error, Error Code is %d, Error Message is %d", MERROR::SOCKET_SYSTEM_ERROR, ec.message());
					return MERROR::SOCKET_SYSTEM_ERROR;
				}
			}
		}
		return 0;
	}


}
