#include "MAsioIOServerPool.h"

namespace MUZI::net::io_pool
{
	MAsioIOServerPool::MAsioIOServerPool(size_t size)
		:m_iocontextes(size),
		m_iocontext_index(0),
	{
	}
	IOContext& MAsioIOServerPool::getIOContext()
	{
		// TODO: 在此处插入 return 语句
	}
	void MAsioIOServerPool::stop()
	{
	}
	MAsioIOServerPool::~MAsioIOServerPool()
	{
	}
}
