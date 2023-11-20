#include"MSession.h"
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>

namespace MUZI::net::coroutine
{
	String MSession::createUUID()
	{
		static boost::uuids::random_generator rgen;
		return boost::uuids::to_string(rgen());
	}

	bool MSession::isWriteCompleted()
	{
		return this->send_pending;
	}
	bool MSession::isReadCompleted()
	{
		return this->recv_pending;
	}
	bool MSession::isClose()
	{
		return this->close_flag;
	}

	const String& MSession::getUUID()
	{
		return this->uuid;
	}

	MSession::MSession(IOContext& io_context)
		:socket(io_context),
		send_pending(false),
		recv_pending(false),
		uuid(MSession::createUUID()),
		recv_tmp_package(new MRecvMsgNode()),
		recv_tmp_buff(new MRecvMsgNode()),
		head_parse(false),
		close_flag(false)
	{

	}
	MSession::~MSession()
	{

	}

	TCPSocket& MSession::getSocket()
	{
		return this->socket;
	}
	RecvMsgPackage MSession::getPopFrontRecvMsg()
	{

	}
}