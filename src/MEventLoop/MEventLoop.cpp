#include "MEventLoop.h"

namespace MUZI::_event
{
	MEventLoop::MEventLoop():
		m_thread_id(std::this_thread::get_id())
	{

	}

	void MEventLoop::push_back(const MEvent& event)
	{
		std::lock_guard glock(this->m_rmutex);
		this->m_event_queue.push(event);

	}

	void MEventLoop::run()
	{

	}
}


