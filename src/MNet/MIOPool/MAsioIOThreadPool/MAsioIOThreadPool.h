#pragma once
#ifndef __MUZI_MASIOTHREAD_POOL_H__
#define __MUZI_MASIOTHREAD_POOL_H__
#include"MNet/MNetBase.h"
#include"MDesignModel/MSingleton/MSingleton.h"
#include<thread>
#include<vector>

namespace MUZI::net::io_pool
{
	class MAsioThreadPool: public singleton::MSingleton<MAsioThreadPool>
	{
	public:
		friend class singleton::MSingleton<MAsioThreadPool>;
	public:
		MAsioThreadPool(const MAsioThreadPool& pool) = delete;
		~MAsioThreadPool();
	public:
		MAsioThreadPool& operator=(const MAsioThreadPool& pool) = delete;
	private:
		MAsioThreadPool(int thread_num = std::thread::hardware_concurrency());

	public:
		IOContext& getIOContext();
		void stop();
	private:
		IOContext m_io_context;
		std::unique_ptr<IOContextWork> m_work;
		std::vector<std::thread> m_threads;
	};



}

#endif // !__MUZI_MASIOTHREAD_POOL_H__
