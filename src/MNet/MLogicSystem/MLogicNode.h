#pragma once
#ifndef __MUZI_MLOGICNODE_H__
#define __MUZI_MLOGICNODE_H__

#include"MNet/MAsyncSocket/MSession.h",
#include"MNet/MMsgNode/MRecvMsgNode.h"
#include"MNet/MMsgNode/MSendMsgNode.h"
#include<memory>

namespace MUZI::net
{
	template<typename T>
	class MLogicNode
	{
	public:
		//friend class LogicSystem;
	public:
		MLogicNode()
		{}
		MLogicNode(async::NetAsyncIOAdapt& session, RecvMsgPackage package)
			:session(session), package(package)
		{}
	
	public:
		inline T& getSession()
		{
			return this->session;
		}
		inline RecvMsgPackage& getPackage()
		{
			return this->package;
		}
	private:
		T session;
		RecvMsgPackage package;

	};

	template<typename T>
	using MLogicPackage = std::shared_ptr<MLogicNode<T>>;
}

#endif // !__MUZI_MLOGICNODE_H__
