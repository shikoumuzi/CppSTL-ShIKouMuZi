#include"MTimer.h"
#include<thread>
#include<chrono>
#include<stdint.h>

namespace MUZI
{
	// ����״̬
	struct MTimerTaskStatus
	{

	};
	// ��ʱ�߳�
	struct MTimerThread
	{

	};

	using MTimerMainThread = std::thread;

	class MTimerData
	{
	public:
		static struct MTimerTaskStatus tasks_status[__MUZI_MTIMER_MAX_TASK_SIZE__];
		static struct MTimerThread tasks_threads[__MUZI_MTIMER_MAX_THREAD_SIZE__];
	public:
		static MTimerMainThread main_thread;

	};

	MTimer* MTimer::getMTimer()
	{

	}

	MTimer::MTimer()
	{

	}
	MTimer::~MTimer()
	{

	}
// ת������
	constexpr uint32_t MTimer::millsecond_2_microsecond(uint32_t microsecond)
	{

	}
	constexpr uint32_t MTimer::second_2_microsecond(uint32_t microsecond)
	{

	}
// ��������
	Task MTimer::set_task(uint32_t microsecond, int stat)// ���ö�ʱ����,����һ�����
	{

	}
	Task MTimer::start_task(Task* task_flag)// ��ʼ����
	{

	}
	Task MTimer::stop_task(Task* task_flag)// ��ͣ����
	{

	}
	Task MTimer::call_task(const Task* task_flag)// ���ж�ʱ���񣬵���ǰ���񵽹涨����ʱ���ض�Ӧ��ʶ��
	{

	}
	int MTimer::del_task(Task* task_flag)// ȡ����ʱ����,�ɹ����ض�Ӧ��ʶ���ţ�ʧ�ܷ���-1
	{

	}
// ��ȡ��ʱ��״̬
	inline size_t MTimer::size()// ������
	{

	}
	inline size_t MTimer::maxsize()// ���������
	{

	}
	inline bool MTimer::is_full()
	{

	}
};