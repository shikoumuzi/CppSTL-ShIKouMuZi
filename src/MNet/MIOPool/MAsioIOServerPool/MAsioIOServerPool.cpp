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
		// TODO: �ڴ˴����� return ���
	}
	void MAsioIOServerPool::stop()
	{
	}
	MAsioIOServerPool::~MAsioIOServerPool()
	{
	}
}
