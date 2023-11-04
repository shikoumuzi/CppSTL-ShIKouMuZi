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
}