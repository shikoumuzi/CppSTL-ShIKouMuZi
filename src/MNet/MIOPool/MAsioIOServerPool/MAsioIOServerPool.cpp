#include "MAsioIOServerPool.h"

namespace MUZI::net::io_pool
{
	MAsioIOServerPool::MAsioIOServerPool(size_t size)
		:m_iocontextes(size),
		m_works(size),
		m_iocontext_index(0)
	{
		for (size_t i = 0; i < size; ++i)
		{
			this->m_works[i] = IOContextWorkPackage(new IOContextWork(this->m_iocontextes[i]));
			this->m_threads.emplace_back(
				[this, i]() {
					this->m_iocontextes[i].run();
				});
		}
	}
	IOContext& MAsioIOServerPool::getIOContext()
	{
		auto& context = this->m_iocontextes[this->m_iocontext_index++];
		if (this->m_iocontext_index >= this->m_iocontextes.size())
		{
			this->m_iocontext_index = 0;
		}
		return context;
	}
	void MAsioIOServerPool::stop()
	{
		for (auto& x : this->m_works)
		{
			x.reset();
		}
		for (auto& x : this->m_threads)
		{
			x.join();
		}
	}
	MAsioIOServerPool::~MAsioIOServerPool()
	{
		this->stop();
	}
}
