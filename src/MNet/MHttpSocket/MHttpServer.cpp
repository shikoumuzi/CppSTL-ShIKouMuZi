#include"MHttpServer.h"
#include<map>
#include<rapidjson/document.h>
#include<rapidjson/stringbuffer.h>
#include<rapidjson/writer.h>
#include<string_view>
namespace MUZI::net::http
{
	int MHttpServer::HttpConnection::DirectoryMode = 0;
	class HttpConnectionData
	{
	public:
		HttpConnectionData(
			class MHttpServer* parent,
			TCPSocket& socket,
			size_t buffer_size = __MUZI_MHTTPSERVER_BUFFER_SIZE__,
			size_t time_out = __MUZI_MHTTPSERVER_TIMEOUT_VALUE__) :
			parent(parent),
			m_socket(std::move(socket)),
			m_buffer(buffer_size),
			m_timer(this->m_socket.get_executor(), std::chrono::seconds(time_out)), // 设置超时时间，如果超过该值还没处理完http则返回
			m_uuid(MHttpServer::HttpConnection::createUUID())
		{}
		HttpConnectionData(
			class MHttpServer* parent,
			TCPSocket&& socket,
			size_t buffer_size = __MUZI_MHTTPSERVER_BUFFER_SIZE__,
			size_t time_out = __MUZI_MHTTPSERVER_TIMEOUT_VALUE__) :
			parent(parent),
			m_socket(std::move(socket)),
			m_buffer(buffer_size),
			m_timer(this->m_socket.get_executor(), std::chrono::seconds(time_out)), // 设置超时时间，如果超过该值还没处理完http则返回
			m_uuid(MHttpServer::HttpConnection::createUUID())
		{}
	public:
		TCPSocket m_socket;
		boost::beast::flat_buffer m_buffer;
		boost::beast::http::request<boost::beast::http::dynamic_body> m_request; // 动态包体
		boost::beast::http::response<boost::beast::http::dynamic_body> m_response;
		boost::beast::net::steady_timer m_timer;
		String m_uuid;
		class MHttpServer* parent;
	};

	class HttpServerData
	{
	public:
		HttpServerData() :
			m_io_context_new_flag(true),
			m_io_context(new IOContext())
		{
		}
		HttpServerData(IOContext& io_context) :
			m_io_context_new_flag(false),
			m_io_context(&io_context)
		{
		}
		~HttpServerData()
		{
			if (this->m_io_context_new_flag)
			{
				delete this->m_io_context;
				this->m_io_context = nullptr;
			}
		}
	public:
		std::map<String, MHttpServer::FilePath> m_file_mapping;
		std::map<String, std::shared_ptr<MHttpServer::HttpConnection>> m_connection_mapping;
		IOContext* m_io_context;
		bool m_io_context_new_flag;
	};

	MHttpServer::HttpConnection::HttpConnection(
		class MHttpServer* parent,
		TCPSocket& socket,
		size_t buffer_size,
		size_t time_out) :
		m_data(new HttpConnectionData(parent, socket, buffer_size, time_out))
	{
	}

	MHttpServer::HttpConnection::HttpConnection(
		MHttpServer* parent,
		TCPSocket&& socket,
		size_t buffer_size,
		size_t time_out) :
		m_data(new HttpConnectionData(parent, socket, buffer_size, time_out))
	{
	}

	MHttpServer::HttpConnection::~HttpConnection()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	String MHttpServer::HttpConnection::createUUID()
	{
		static boost::uuids::random_generator rgen;
		return boost::uuids::to_string(rgen());
	}

	const String& MHttpServer::HttpConnection::getUUID()
	{
		return this->m_data->m_uuid;
	}

	void MHttpServer::HttpConnection::start()
	{
		this->readRequest();
		this->checkDeadline();
	}

	void MHttpServer::HttpConnection::close()
	{
		this->m_data->m_socket.close();
		this->m_data->parent->m_data->m_connection_mapping.erase(this->getUUID());
	}

	void MHttpServer::HttpConnection::readRequest()
	{
		auto self = shared_from_this();
		this->m_data->m_request.clear();
		boost::beast::http::async_read(this->m_data->m_socket, this->m_data->m_buffer, this->m_data->m_request,
			[self](const EC& ec, std::size_t byte_transferred)
			{
				if (!ec)
				{
					self->processRequest();
				}
			});
	}

	void MHttpServer::HttpConnection::checkDeadline()
	{
		auto self = shared_from_this();
		// 该智能指针是为了保证当超时时，this对象不为空，相当于采用闭包延长connection生命周期
		this->m_data->m_timer.async_wait(
			[self](const EC& ec)
			{
				self->m_data->m_socket.close();
				self->m_data->parent->m_data->m_connection_mapping.erase(self->getUUID());
			});
	}

	void MHttpServer::HttpConnection::processRequest()
	{
		this->m_data->m_response.version(this->m_data->m_request.version());
		this->m_data->m_response.keep_alive(false); // http协议为短链接
		switch (this->m_data->m_request.method())
		{
		case HttpVerb::get:
		{
			this->m_data->m_response.result(HttpStatus::ok);
			this->m_data->m_response.set(HttpField::server, "Beast");
			this->createGetResponse();
			break;
		}
		case HttpVerb::post:
		{
			this->m_data->m_response.result(HttpStatus::ok);
			this->m_data->m_response.set(HttpField::server, "Beast");
			break;
		}
		case HttpVerb::patch:
			break;
		case HttpVerb::delete_:
			break;
		default:
			// 如果都不匹配
			this->m_data->m_response.result(HttpStatus::bad_request);
			this->m_data->m_response.set(HttpField::content_type, "text/plain");
			boost::beast::ostream(this->m_data->m_response.body())
				<< "Invalid request-method '"
				<< this->m_data->m_request.method_string() << "'";
			break;
		}

		this->writeResponse();
	}

	void MHttpServer::HttpConnection::createGetResponse()
	{
		// 查看请求路径， 假设/count表示请求了多少次
		//if (this->m_data->m_request.target() == "/count")
		//{
		//	this->m_data->m_reponse.set(HttpField::content_type, "text/html");
		//	boost::beast::ostream(this->m_data->m_reponse.body())
		//		<< "Invalid request-method '"
		//		<< this->m_data->m_request.method_string() << "'";
		//}
		// 查看请求路径
		auto result_iter = this->m_data->parent->m_data->m_file_mapping.find(String(this->m_data->m_request.target()));
		if (result_iter == this->m_data->parent->m_data->m_file_mapping.end())
		{
			this->m_data->m_response.result(HttpStatus::not_found);
			this->m_data->m_response.set(HttpField::content_type, "text/plain");
			boost::beast::ostream(this->m_data->m_response.body())
				<< "File not found\r\n";
			return;
		}
		String mail_body;
		boost::filesystem::load_string_file(result_iter->second.c_str(), mail_body);
		boost::beast::ostream(this->m_data->m_response.body()) << mail_body;
	}

	void MHttpServer::HttpConnection::createPostResponse()
	{
		auto result_iter = this->m_data->parent->m_data->m_file_mapping.find(String(this->m_data->m_request.target()));
		if (result_iter == this->m_data->parent->m_data->m_file_mapping.end())
		{
			this->m_data->m_response.result(HttpStatus::not_found);
			this->m_data->m_response.set(HttpField::content_type, "text/plain");
			boost::beast::ostream(this->m_data->m_response.body())
				<< "File not found\r\n";
			return;
		}
		auto& body = this->m_data->m_request.body();
		auto body_str = boost::beast::buffers_to_string(body.data());
		this->m_data->m_response.set(HttpField::content_type, "text/json");
		rapidjson::Document doc;
		rapidjson::StringBuffer response_body_buff;
		rapidjson::Writer<rapidjson::StringBuffer> json_writer(response_body_buff);
		doc.Parse(body_str.c_str());

		if (doc.HasParseError())
		{
			MLog::w("MHttpServer::HttpConnection::createPostResponse", "Failed to parse json data");

			json_writer.StartObject();
			json_writer.Key("error");
			json_writer.Int(1001);
			json_writer.EndObject();
			boost::beast::ostream(this->m_data->m_response.body()) << response_body_buff.GetString();

			return;
		}

		String mail_body;
		boost::filesystem::load_string_file(result_iter->second.c_str(), mail_body);
		boost::beast::ostream(this->m_data->m_response.body()) << mail_body;
	}

	void MHttpServer::HttpConnection::writeResponse()
	{
		auto self = shared_from_this();
		this->m_data->m_response.content_length(this->m_data->m_response.body().size());
		boost::beast::http::async_write(
			this->m_data->m_socket,
			this->m_data->m_response,
			[self](const EC& ec, size_t bytes_transafered)
			{
				if (ec)
				{
					// 只关闭发送端，尽量不要全关，因为有四次挥手的存在，若客户端未响应，而服务器全部关闭时会存在僵尸连接，
					self->m_data->m_socket.shutdown(TCPSocket::shutdown_send);
					self->m_data->m_timer.cancel();
				}
			}
		);
	}

	MHttpServer::MHttpServer() :
		m_data(new HttpServerData())
	{
	}

	MHttpServer::MHttpServer(IOContext& io_context) :
		m_data(new HttpServerData(io_context))
	{
	}

	MHttpServer::~MHttpServer()
	{
		if (this->m_data != nullptr)
		{
			delete this->m_data;
			this->m_data = nullptr;
		}
	}

	void MHttpServer::accept(TCPAcceptor& acceptor, TCPSocket& socket)
	{
		acceptor.async_accept(socket,
			[this, &acceptor, &socket](const EC& ec)
			{
				if (!ec)
				{
					auto connect_ptr = std::make_shared<HttpConnection>(this, std::move(socket));
					this->m_data->m_connection_mapping[connect_ptr->getUUID()] = connect_ptr;
					connect_ptr->start();
				}
				this->accept(acceptor, socket);
			});
	}

	bool MHttpServer::resisterPath(String& target, const FilePath& dir_path, int deepth)
	{
		if (deepth > 9)
		{
			return false;
		}
		if (!dir_path.is_absolute())
		{
			return false;
		}
		boost::filesystem::recursive_directory_iterator iter(dir_path);
		while (iter != boost::filesystem::recursive_directory_iterator())
		{
			if (boost::filesystem::is_regular_file(iter->path()))
			{
				auto file_path = iter->path();
				if (file_path.extension() != "html" or
					file_path.stem() != target)
				{
					continue;
				}
				this->m_data->m_file_mapping[target] = file_path;
				return true;
			}
			else if (boost::filesystem::is_directory(iter->path()))
			{
				return this->resisterPath(target, iter->path(), deepth + 1);
			}
		}

		return false;
	}

	bool MHttpServer::registerPath(String& target, const FilePath& file_path)
	{
		if (!boost::filesystem::exists(file_path) or
			!boost::filesystem::is_regular_file(file_path) or
			file_path.extension() != "html" or
			file_path.stem() != target)
		{
			return false;
		}
		this->m_data->m_file_mapping[target] = file_path;
		return true;
	}

	void MHttpServer::registerPaths(const FilePath& dir_path, int deepth)
	{
		if (deepth > 9)
		{
			return;
		}
		if (!boost::filesystem::exists(dir_path) or
			!boost::filesystem::is_directory(dir_path) or
			!dir_path.is_absolute())
		{
			return;
		}
		boost::filesystem::recursive_directory_iterator iter(dir_path);
		while (iter != boost::filesystem::recursive_directory_iterator())
		{
			if (boost::filesystem::is_regular_file(iter->path()))
			{
				auto file_path = iter->path();
				if (file_path.extension() != "html")
				{
					continue;
				}
				this->m_data->m_file_mapping[file_path.stem().string()] = file_path;
			}
			else if (boost::filesystem::is_directory(iter->path()))
			{
				this->registerPaths(iter->path(), deepth + 1);
			}
		}
	}

	Map<String, MHttpServer::FilePath>& MHttpServer::getTargetMapping()
	{
		return this->m_data->m_file_mapping;
	}

	MHttpServer::FilePath MHttpServer::getFilePath(String& target)
	{
		auto iter = this->m_data->m_file_mapping.find(target);
		if (iter == this->m_data->m_file_mapping.end())
		{
			return FilePath();
		}
		return iter->second;
	}
}