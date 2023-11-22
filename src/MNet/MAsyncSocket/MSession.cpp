#include"MSession.h"
namespace MUZI::net::async
{
	String MSession::createUUID()
	{
		static boost::uuids::random_generator rgen;
		return boost::uuids::to_string(rgen());
	}

	MSession::MSession(IOContext& io_context)
		:socket(io_context),
		send_pending(false),
		recv_pending(false),
		uuid(MSession::createUUID()),
		recv_tmp_package(new MRecvMsgNode()),
		recv_tmp_buff(new MRecvMsgNode()),
		head_parse(false),
		m_strand(io_context.get_executor())
	{}

	MSession::~MSession()
	{
		this->socket.close();
	}

	inline bool MSession::isWriteCompleted()
	{
		return this->send_pending;
	}

	inline bool MSession::isReadCompleted()
	{
		return this->recv_pending;
	}

	const String& MSession::getUUID()
	{
		return this->uuid;
	}

	TCPSocket& MSession::getSocket()
	{
		return this->socket;
	}

	RecvMsgPackage MSession::getPopFrontRecvMsg()
	{
		while (this->recv_completed_queue.empty())
		{

		}
		auto ret_msg = this->recv_completed_queue.front();
		this->recv_completed_queue.pop();
		return *ret_msg;
		
	}
}