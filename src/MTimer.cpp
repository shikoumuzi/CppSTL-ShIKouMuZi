#include"MTimer.h"
#include<thread>
#include<chrono>
#include<stdint.h>

namespace MUZI
{
	// 任务状态
	struct MTimerTaskStatus
	{

	};
	// 定时线程
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
// 转换函数
	constexpr uint32_t MTimer::millsecond_2_microsecond(uint32_t microsecond)
	{

	}
	constexpr uint32_t MTimer::second_2_microsecond(uint32_t microsecond)
	{

	}
// 操作函数
	Task MTimer::set_task(uint32_t microsecond, int stat)// 设置定时任务,返回一个标号
	{

	}
	Task MTimer::start_task(Task* task_flag)// 开始任务
	{

	}
	Task MTimer::stop_task(Task* task_flag)// 暂停任务
	{

	}
	Task MTimer::call_task(const Task* task_flag)// 呼叫定时任务，当当前任务到规定条件时返回对应标识符
	{

	}
	int MTimer::del_task(Task* task_flag)// 取消定时任务,成功返回对应标识符号，失败返回-1
	{

	}
// 获取定时器状态
	inline size_t MTimer::size()// 任务数
	{

	}
	inline size_t MTimer::maxsize()// 最大任务数
	{

	}
	inline bool MTimer::is_full()
	{

	}
};