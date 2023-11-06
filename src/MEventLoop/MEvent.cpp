#include"MEvent.h"

namespace MUZI::_event
{
	MEvent::MEvent(): 
		m_priority(10), 
		m_event_type(EVENT_TYPE::NULL_EVENT)
	{
	}

	bool MEvent::operator()(const MEvent& event)
	{
		return this->m_priority < event.m_priority;
	}

	std::weak_ordering MEvent::operator<=>(const MEvent& event)
	{
		if (this->m_priority < event.m_priority)
		{
			return std::weak_ordering::less;
		}
		else if (this->m_priority > event.m_priority)
		{
			return std::weak_ordering::greater;
		}
		else
		{
			return std::weak_ordering::equivalent;
		}
	}

	bool MEvent::operator==(const MEvent& event)
	{
		return this->m_priority == event.m_priority;
	}

	std::weak_ordering MEvent::operator<=>(const MEvent& event) const
	{
		if (this->m_priority < event.m_priority)
		{
			return std::weak_ordering::less;
		}
		else if (this->m_priority > event.m_priority)
		{
			return std::weak_ordering::greater;
		}
		else
		{
			return std::weak_ordering::equivalent;
		}
	}

	bool MEvent::operator==(const MEvent& event) const
	{
		return this->m_priority == event.m_priority;
	}

	void MEvent::setMouseEvent(event_t event, short abs_x, short abs_y)
	{
		if (event > MEvent::MOUSE_TYPE::START_MOUSE_EVENT && event < MEvent::MOUSE_TYPE::END_MOUSE_EVENT)
		{
			this->m_event_type = MEvent::EVENT_TYPE::MOUSE;
			this->m_event_msg = std::make_shared<void*>(static_cast<void*>(new MEvent::MouseEventMsg));
			static_cast<MEvent::MouseEventMsg*>(*this->m_event_msg.get())->abs_x = abs_x;
			static_cast<MEvent::MouseEventMsg*>(*this->m_event_msg.get())->abs_y = abs_y;
			static_cast<MEvent::MouseEventMsg*>(*this->m_event_msg.get())->event_type = event;
		}
		return;
	}

	void MEvent::setKeyBoardEvent(event_t event)
	{
	}

	void MEvent::setSignalTriggerEvent(event_t event)
	{
	}


}