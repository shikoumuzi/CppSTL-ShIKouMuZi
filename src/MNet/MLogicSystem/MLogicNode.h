#pragma once
#ifndef __MUZI_MLOGICNODE_H__
#define __MUZI_MLOGICNODE_H__

#include"MNet/MAsyncSocket/MSession.h",
#include"MNet/MMsgNode/MRecvMsgNode.h"
#include"MNet/MMsgNode/MSendMsgNode.h"
#include<memory>

namespace MUZI::net
{
	class MLogicNode
	{
	public:
		friend class LogicSystem;
	public:
		MLogicNode()
		{}
		MLogicNode(async::NetAsyncIOAdapt& adapt, RecvMsgPackage package)
			:adapt(adapt), package(package)
		{}
	
	public:
		inline async::NetAsyncIOAdapt& getAdapt()
		{
			return this->adapt;
		}
		inline RecvMsgPackage& getRecvMsgPackage()
		{
			return this->package;
		}
	private:
		async::NetAsyncIOAdapt adapt;
		RecvMsgPackage package;

	};

	using MLogicPackage = std::shared_ptr<MLogicNode>;
}

#endif // !__MUZI_MLOGICNODE_H__
