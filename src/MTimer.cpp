#include"MTimer.h"
#include<thread>
#include<chrono>
#include<mutex>
#include<atomic>
#include<stdint.h>
#include<errno.h>

namespace MUZI
{
// 数据结构
	class MTimerData* __MUZI_MTIMER_DATA__ = nullptr;
	static std::mutex* __MUZI_MTIMER_INIT_LOCK__ = new std::mutex;
	// 任务状态
	struct MTimerTaskStatus
	{
		uint64_t cycle_times;// 时间周期数
	};

	// 定时线程
	using ChildThreadFunction = void(*)(void*);
	struct MTimerChildThread
	{
		size_t min_pos;// 负责范围内的最低值
		size_t max_pos;// 负责范围内的最高值
		std::thread child_th;
		std::condition_variable* cond;
		std::unique_lock<std::mutex>* cond_unique_lock;
	};
	void __MUZI_MTIMER_CHILD_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerChildThread& child_thread = *static_cast<struct MTimerChildThread*>(job_ptr);
		while (1)
		{
			// 子线程 处于等待状态，等待主线程抵达下一时间单元的命令
			child_thread.cond->wait(*child_thread.cond_unique_lock);
			for (size_t i = child_thread.min_pos; i <= child_thread.max_pos; ++i)
			{
				// do something
				__MUZI_MTIMER_DATA__->tasks_status[i];
			}
		}
	}


	// 计时线程
	using MainThreadFunction = void(*)(void*);
	struct MTimerMainThread
	{
		std::thread main_th;
		std::condition_variable cond;
		std::unique_lock<std::mutex> cond_unique_lock;
		std::chrono::steady_clock::time_point work_start_time;
		std::chrono::microseconds timeout_standnrd;
	};
	void __MUZI_MTIMER_MAIN_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerMainThread& main_thread = *static_cast<struct MTimerMainThread*>(job_ptr);
		main_thread.work_start_time = std::chrono::steady_clock::now();
		main_thread.cond_unique_lock.lock();
		// 根据最小单位叠加，等到抵达下一个单元的时候向其他线程提醒动作
		while ((main_thread.cond.
			wait_for(main_thread.cond_unique_lock, main_thread.timeout_standnrd) == std::cv_status::timeout)
			|| errno == ETIMEDOUT)
		{
			main_thread.cond.notify_all();
		}
	}

	class MTimerData
	{
	public:
		static MTimerData* getMTimerData()
		{
			__MUZI_MTIMER_INIT_LOCK__->lock();
			if (__MUZI_MTIMER_DATA__ != nullptr)
			{
				__MUZI_MTIMER_DATA__ = new MTimerData;
				size_t i = 0;
				// 初始化任务
				for (; i < __MUZI_MTIMER_MAX_TASK_SIZE__; ++i)
				{

				}
				// 初始化主线程
				static struct MTimerMainThread& main_thread = __MUZI_MTIMER_DATA__->main_thread;

				// 设定任务开始时间
				main_thread.work_start_time = std::chrono::steady_clock::now();
				// 初始化为10微秒作为一个时间单位
				main_thread.timeout_standnrd = std::chrono::duration<uint64_t, std::micro>(10);
				main_thread.main_th = std::move(std::thread(__MUZI_MTIMER_MAIN_THREAD_FUN__, &__MUZI_MTIMER_DATA__->main_thread));

				// 初始化子线程
				struct MTimerChildThread* child_threads = __MUZI_MTIMER_DATA__->tasks_threads;
				for (i = 0; i < __MUZI_MTIMER_MAX_THREAD_SIZE__; ++i)
				{
					child_threads[i].cond = &main_thread.cond;
					child_threads[i].cond_unique_lock = &main_thread.cond_unique_lock;
					child_threads[i].max_pos = 0;
					child_threads[i].min_pos = 0;
					child_threads[i].child_th = std::move(std::thread(__MUZI_MTIMER_CHILD_THREAD_FUN__, &child_threads[i]));
				}

				__MUZI_MTIMER_INIT_LOCK__->unlock();
				delete __MUZI_MTIMER_INIT_LOCK__;
				__MUZI_MTIMER_DATA__ = nullptr;
			}
			else
			{
				if (__MUZI_MTIMER_INIT_LOCK__ != nullptr)
					__MUZI_MTIMER_INIT_LOCK__->unlock();
				
				return __MUZI_MTIMER_DATA__;
			}

		}
	public:
		static struct MTimerTaskStatus tasks_status[__MUZI_MTIMER_MAX_TASK_SIZE__];
		static struct MTimerChildThread tasks_threads[__MUZI_MTIMER_MAX_THREAD_SIZE__];
	public:
		static struct MTimerMainThread main_thread;
	};

	MTimer* MTimer::getMTimer()
	{

	}
	void MTimer::delMTimer(MTimer* mtimer)
	{

	}
//初始化函数
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