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


}