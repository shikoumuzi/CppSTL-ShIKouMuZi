#pragma once
#ifndef __MUZI_MLOGICNODE_H__
#define __MUZI_MLOGICNODE_H__

#include"MNet/MAsyncSocket/MSession.h",
#include"MNet/MMsgNode/MRecvMsgNode.h"
#include"MNet/MMsgNode/MSendMsgNode.h"

namespace MUZI::net
{
	class MLogicNode
	{
	public:
		friend class LogicSystem;
	public:
		MLogicNode(async::NetAsyncIOAdapt adapt, RecvMsgPackage package)
		{}
	private:
		async::NetAsyncIOAdapt adapt;
		RecvMsgPackage package;

	};
}

#endif // !__MUZI_MLOGICNODE_H__
