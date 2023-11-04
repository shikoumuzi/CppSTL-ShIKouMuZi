#include "MEventLoop.h"

namespace MUZI::_event
{
	MEventLoop::MEventLoop():
		m_thread_id(std::this_thread::get_id()),
		m_work_flag(false),
		m_atomic_mode(false)
	{

	}

	void MEventLoop::push_back(const MEvent& event)
	{
		if(this->m_atomic_mode)
		{ 
			this->m_amutex.lock();
			this->m_event_queue.push(event);
			this->m_amutex.unlock();
		}
		else
		{
			this->m_rmutex.lock();
			this->m_event_queue.push(event);
			this->m_rmutex.unlock();
		}

	}

	void MEventLoop::run()
	{

		MTime start_time = std::chrono::system_clock::now();
		int event_ctrl_times = 0;

		while (this->m_work_flag)
		{
			
			if (this->m_event_queue.size() > 0)
			{
				// ������һ��Ƶ�ʵ��¼�����ʱ�������ԭ����������һ��Ƶ��ʱ���򴥷��ݹ����� Ĭ��Ϊ�ݹ���
				if (this->m_atomic_mode)
				{
					this->m_amutex.lock();
					MEvent top_event = this->m_event_queue.top();
					this->ctrlEvent(top_event);
					this->m_event_queue.pop();
					this->m_amutex.unlock();
				}
				else
				{
					this->m_rmutex.lock();
					MEvent top_event = this->m_event_queue.top();
					this->ctrlEvent(top_event);
					this->m_event_queue.pop();
					this->m_rmutex.unlock();
				}
				// �ú��������Զ������¼�����Ƶ�ʣ����л�
				this->analyizeCtrlMode();
				
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			
		}
	}
	void MEventLoop::ctrlEvent(const MEvent& event)
	{
		switch (event.m_event_type)
		{
		case MEvent::EVENT_TYPE::MOUSE:
		{
			MEvent::MouseEventMsg* event_msg = static_cast<MEvent::MouseEventMsg*>(*event.m_event_msg.get());
			
			break;
		}
		case MEvent::EVENT_TYPE::KEY_BOARD:
		{
			MEvent::KeyBoardEventMsg* event_msg = static_cast<MEvent::KeyBoardEventMsg*>(*event.m_event_msg.get());

			break;
		}
		case MEvent::EVENT_TYPE::SIGNAL_TRIGGER:
		{
			MEvent::SignalTriggerMsg* event_msg = static_cast<MEvent::SignalTriggerMsg*>(*event.m_event_msg.get());

			break;
		}
		default:
			break;
		}
	}
	void MEventLoop::analyizeCtrlMode()
	{

	}
}


