#include "MEventLoop.h"

namespace MUZI::_event
{
	MEventLoop::MEventLoop():
		m_thread_id(std::this_thread::get_id()),
		m_work_flag(false)
	{

	}

	void MEventLoop::push_back(const MEvent& event)
	{
		std::lock_guard glock(this->m_rmutex);
		this->m_event_queue.push(event);

	}

	void MEventLoop::run()
	{
		while (this->m_work_flag)
		{
			std::lock_guard glock(this->m_rmutex);
			MEvent top_event = this->m_event_queue.top();
			switch (top_event.m_event_type)
			{
			case MEvent::EVENT_TYPE::MOUSE:
			{
				MEvent::MouseEventMsg* event_msg = static_cast<MEvent::MouseEventMsg*>(*top_event.m_event_msg.get());
				switch (event_msg->event_type)
				{
				case MEvent::MOUSE_TYPE::CLICK:
				{
					break;
				}
				case MEvent::MOUSE_TYPE::DOUBLE_CLICK:
				{
					break;
				}
				case MEvent::MOUSE_TYPE::DROP:
				{
					break;
				}
				case MEvent::MOUSE_TYPE::HOLD:
				{
					break;
				}
				case MEvent::MOUSE_TYPE::RELEASE:
				{
					break;
				}
				default:
					break;
				}
				break;
			}
			case MEvent::EVENT_TYPE::KEY_BOARD:
			{
				MEvent::KeyBoardEventMsg* event_msg = static_cast<MEvent::KeyBoardEventMsg*>(*top_event.m_event_msg.get());

				break;
			}
			case MEvent::EVENT_TYPE::SIGNAL_TRIGGER:
			{
				MEvent::SignalTriggerMsg* event_msg = static_cast<MEvent::SignalTriggerMsg*>(*top_event.m_event_msg.get());

				break;
			}
			default:
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			
		}
	}
}


