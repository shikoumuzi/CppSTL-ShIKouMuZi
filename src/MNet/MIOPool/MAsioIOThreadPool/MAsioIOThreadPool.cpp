#include"MAsioIOThreadPool.h"


namespace MUZI::net::io_pool
{

	MAsioThreadPool::MAsioThreadPool(int thread_num)
		:m_work(new IOContextWork(this->m_io_context))
	{
		this->m_threads.reserve(thread_num);
		for (int i = 0; i < thread_num; ++i)
		{
			this->m_threads.emplace_back(
				[this]()
				{
					this->getIOContext().run();
				}
			);
		}
	}

	MAsioThreadPool::~MAsioThreadPool()
	{
	}
	IOContext& io_pool::MAsioThreadPool::getIOContext()
	{
		return this->m_io_context;
	}
	void io_pool::MAsioThreadPool::stop()
	{
		this->m_work.reset();
		for (auto& t : this->m_threads)
		{
			t.join();
		}
		this->m_io_context.stop();
	}
}