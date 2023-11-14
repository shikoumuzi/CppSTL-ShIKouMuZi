#pragma once
#ifndef __MUZI_MASIO_SERVER_POOL_H__
#define __MUZI_MASIO_SERVER_POOL_H__

#include<stdint.h>
#include<thread>
#include"MSingleton/MSingleton.h"
#include<boost/asio.hpp>
#include<vector>
#include"MNet/MNetBase.h"

namespace MUZI::net::io_pool
{
	class MAsioIOServerPool: public singleton::MSingleton<MAsioIOServerPool>
	{
	private:
		MAsioIOServerPool(size_t size = std::thread::hardware_concurrency());
		MAsioIOServerPool(const MAsioIOServerPool&) = delete;
		MAsioIOServerPool(MAsioIOServerPool&& pool);
	private:
		MAsioIOServerPool& operator=(const MAsioIOServerPool&) = delete;
	public:
		IOContext& getIOContext();
		void stop();
	public:
		~MAsioIOServerPool();
	

	private:
		std::vector<IOContext> m_iocontextes;
		std::vector<IOContextWorkPackage> m_works;
		std::vector<std::thread> m_threads;
		size_t m_iocontext_index;
	};



}

#endif