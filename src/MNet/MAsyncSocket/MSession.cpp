#include"MSession.h"
namespace MUZI::net::async
{
	String MSession::createUUID()
	{
		static boost::uuids::random_generator rgen;
		return boost::uuids::to_string(rgen());
	}

	MSession::MSession(TCPSocket socket)
		:socket(std::move(socket)),
		send_pending(false),
		recv_pending(false),
		uuid(MSession::createUUID()),
		recv_tmp_package(new MMsgNode(nullptr, __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, true)),
		recv_tmp_buff(new MMsgNode(nullptr, __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, true)),
		head_parse(false)
	{}

	MSession::~MSession()
	{}

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

	MsgPackage MSession::getPopFrontRecvMsg()
	{
		while (this->recv_completed_queue.empty())
		{

		}
		auto ret_msg = this->recv_completed_queue.front();
		this->recv_completed_queue.pop();
		return *ret_msg;
		
	}
}