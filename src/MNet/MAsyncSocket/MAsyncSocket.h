#pragma once
#ifndef __MUZI_MASYNCSOCKET_H__
#define __MUZI_MASYNCSOCKET_H__
#include"MNet/MNetBase.h"
#include<queue>
#include"MNet/MEndPoint/MServerEndPoint.h"
#include"MNet/MEndPoint/MEndPoint.h"
#include<boost/uuid/uuid.hpp>
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include"MMsgNode.h"
#include"MSession.h"
#include<functional>



namespace MUZI::net::async
{

	class MAsyncSocket
	{
	public:
		class MAsyncSocketData;

	public:
		MAsyncSocket();
		~MAsyncSocket();
	public:
		IOContext& getIOContext();
	public:
		int writeToSocket(NetAsyncIOAdapt adapt, String& data);
		int wtiteToSocket(NetAsyncIOAdapt adapt, void* data, uint64_t size);
		int wtiteAllToSocket(NetAsyncIOAdapt adapt, void* data, uint64_t size);

		int readFromSocket(NetAsyncIOAdapt adapt, uint64_t size = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__);
		int readAllFromeSocket(NetAsyncIOAdapt adapt, uint64_t size = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__);

	public:
		int splitSendPackage(NetAsyncIOAdapt adapt, void* data, uint64_t size);
	public:
		void run();
	public:
		class MAsyncSocketData* m_data;

	};


}


#endif // !__MUZI_MASYNCSOCKET_H__
