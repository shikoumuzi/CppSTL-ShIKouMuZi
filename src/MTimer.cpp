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
	static class MTimerData* __MUZI_MTIMER_DATA__ = nullptr;
	static std::mutex* __MUZI_MTIMER_INIT_LOCK__ = new std::mutex;

	class MTimerData
	{
	public:
		static MTimerData* getMTimerData();
		static void delMTimerData(MTimerData** data);
	public:
		static Task& get_free_task();
		//static inline bool get_exit_flag()
		//{
		//	return __MUZI_MTIMER_DATA__->exit_flag;
		//}
		//static inline struct MTimerChildThread& get_tasks_threads(size_t i)
		//{
		//	return __MUZI_MTIMER_DATA__->tasks_threads[i];
		//}
		//static inline struct MTimerTaskStatus& get_tasks_status(size_t i)
		//{
		//	return __MUZI_MTIMER_DATA__->tasks_status[i];
		//}
	public:
		static struct MTimerTaskStatus tasks_status[__MUZI_MTIMER_MAX_TASK_SIZE__];
		static struct MTimerChildThread tasks_threads[__MUZI_MTIMER_MAX_THREAD_SIZE__];
	public:
		static struct MTimerMainThread main_thread;
		static bool exit_flag;
		static size_t task_size;
		static size_t object_num;
	};
	// 任务状态
	enum MTimerTaskStatusFlag
	{
		WORKINGNOW = 0x00000001u,
		NOWORKINGNOW = 0x00000010u,
		FREETASK = 0x10000000u,
		BUSYTASK = 0x01000000u
	};

	struct MTimerTaskStatus
	{
		std::atomic<size_t> work_status;
		std::chrono::steady_clock::time_point task_start_time;// 任务起始时间
		std::atomic<uint64_t> wake_times;// 被唤醒次数
		std::atomic<uint64_t> activate_times;// 激活所需要的次数
		std::unique_lock<std::mutex> task_lock;// 条件变量匹配的锁
		std::condition_variable task_cond;// 激活的条件变量，用以激活call_task函数
		size_t pos;
	};

	// 定时线程
	struct MTimerChildThread
	{
		std::atomic<size_t> min_pos;// 负责范围内的最低值
		std::atomic<size_t> max_pos;// 负责范围内的最高值
		std::thread child_th;
		// 下面是让main_thread唤醒子线程所需要的函数
		std::condition_variable* cond;
		std::unique_lock<std::mutex>* cond_unique_lock;
	};
	void __MUZI_MTIMER_CHILD_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerChildThread& child_thread = *static_cast<struct MTimerChildThread*>(job_ptr);
		while (!__MUZI_MTIMER_DATA__->exit_flag)
		{
			// 子线程 处于等待状态，等待主线程抵达下一时间单元的命令
			child_thread.cond->wait(*child_thread.cond_unique_lock);
			for (size_t i = child_thread.min_pos; i <= child_thread.max_pos; ++i)
			{
				if (__MUZI_MTIMER_DATA__->tasks_status[i].work_status & MTimerTaskStatusFlag::WORKINGNOW)
				{
					struct MTimerTaskStatus& task_data = __MUZI_MTIMER_DATA__->tasks_status[i];
					++task_data.wake_times;
					if (task_data.wake_times % task_data.activate_times == 0)
					{
						task_data.task_cond.notify_all();
					}
				}
			}
		}
	}


	// 计时线程
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
		while (((main_thread.cond.
			wait_for(main_thread.cond_unique_lock, main_thread.timeout_standnrd) == std::cv_status::timeout)
			|| errno == ETIMEDOUT) && (!__MUZI_MTIMER_DATA__->exit_flag))
		{
			main_thread.cond.notify_all();
		}
	}

	// 静态变量初始化和 函数定义
	struct MTimerTaskStatus MTimerData::tasks_status[__MUZI_MTIMER_MAX_TASK_SIZE__];
	struct MTimerChildThread MTimerData::tasks_threads[__MUZI_MTIMER_MAX_THREAD_SIZE__];
	struct MTimerMainThread MTimerData::main_thread;
	bool MTimerData::exit_flag = false;
	size_t MTimerData::task_size = 0;
	size_t MTimerData::object_num = 0;

	MTimerData* MTimerData::getMTimerData()
	{
		__MUZI_MTIMER_INIT_LOCK__->lock();
		if (__MUZI_MTIMER_DATA__ != nullptr)
		{
			__MUZI_MTIMER_DATA__ = new MTimerData;
			size_t i = 0;
			// 初始化任务
			for (; i < __MUZI_MTIMER_MAX_TASK_SIZE__; ++i)
			{
				__MUZI_MTIMER_DATA__->tasks_status[i].work_status = MTimerTaskStatusFlag::FREETASK;
				__MUZI_MTIMER_DATA__->tasks_status[i].wake_times = 0;
				__MUZI_MTIMER_DATA__->tasks_status[i].activate_times = 0;
				__MUZI_MTIMER_DATA__->tasks_status[i].pos = i;
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
			size_t thread_tasks_num = __MUZI_MTIMER_MAX_TASK_SIZE__ / __MUZI_MTIMER_MAX_THREAD_SIZE__;
			size_t start_pos = 0, end_pos = thread_tasks_num;
			for (i = 0; i < __MUZI_MTIMER_MAX_THREAD_SIZE__; ++i)
			{
				child_threads[i].cond = &main_thread.cond;
				child_threads[i].cond_unique_lock = &main_thread.cond_unique_lock;
				child_threads[i].min_pos = start_pos;
				child_threads[i].max_pos = end_pos;
				child_threads[i].child_th = std::move(std::thread(__MUZI_MTIMER_CHILD_THREAD_FUN__, &child_threads[i]));
				start_pos = end_pos + 1;
				end_pos += thread_tasks_num;
			}

			__MUZI_MTIMER_INIT_LOCK__->unlock();
			delete __MUZI_MTIMER_INIT_LOCK__;
			__MUZI_MTIMER_DATA__ = nullptr;
			return __MUZI_MTIMER_DATA__;
		}
		else
		{
			if (__MUZI_MTIMER_INIT_LOCK__ != nullptr)
				__MUZI_MTIMER_INIT_LOCK__->unlock();

			return __MUZI_MTIMER_DATA__;
		}

	}
	void MTimerData::delMTimerData(MTimerData** data)
	{
		__MUZI_MTIMER_DATA__->exit_flag = true;
		__MUZI_MTIMER_DATA__->main_thread.cond.notify_all();
		delete* data;
		*data = nullptr;
		__MUZI_MTIMER_DATA__ = nullptr;
	}

	Task& MTimerData::get_free_task()
	{
		for (size_t i = 0; i < __MUZI_MTIMER_MAX_TASK_SIZE__; ++i)
		{
			if (__MUZI_MTIMER_DATA__->tasks_status[i].work_status & MTimerTaskStatusFlag::FREETASK)
			{
				return __MUZI_MTIMER_DATA__->tasks_status[i];
			}
		}
	}


	// 下面是头文件所看到的函数内容
	MTimer* MTimer::getMTimer()
	{
		return new MTimer();
	}
	void MTimer::delMTimer(MTimer** mtimer)
	{
		delete* mtimer;
		*mtimer = nullptr;
	}
	//初始化函数
	MTimer::MTimer()
	{
		this->data = (__MUZI_MTIMER_DATA__ != nullptr) ? __MUZI_MTIMER_DATA__ : MTimerData::getMTimerData();
		this->data->object_num += 1;
	}
	MTimer::~MTimer()
	{
		this->data->object_num -= 1;
		if (this->data->object_num == 0)
		{
			this->data->delMTimerData(&this->data);
		}
	}
	// 转换函数
	inline uint32_t MTimer::millsecond_2_microsecond(uint32_t millsecond)
	{
		return static_cast<uint32_t>(millsecond / 1000);
	}
	inline uint32_t MTimer::second_2_microsecond(uint32_t second)
	{
		return static_cast<uint32_t>(second / 1000000);
	}
	// 操作函数
	const Task& MTimer::set_task(uint32_t microsecond)// 设置定时任务,返回一个标号
	{
		Task& ret_task = this->data->get_free_task();
		ret_task.activate_times = microsecond / __MUZI_MTIMER_MIN_TIME_UNIT__;
		ret_task.work_status &= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::NOWORKINGNOW;
		__MUZI_MTIMER_DATA__->task_size += 1;
		return ret_task;
	}
	inline const Task& MTimer::start_task(const Task& task_flag)// 开始任务
	{
		//Task& ret_task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		//ret_task.work_status &= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW;
		//return ret_task;
		return (__MUZI_MTIMER_DATA__->tasks_status[task_flag.pos].work_status
			&= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW, task_flag);
	}
	const Task& MTimer::stop_task(const Task& task_flag)// 暂停任务
	{
		return (__MUZI_MTIMER_DATA__->tasks_status[task_flag.pos].work_status
			&= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::NOWORKINGNOW, task_flag);
	}
	bool MTimer::call_task(const Task& task_flag)// 呼叫定时任务，当当前任务到规定条件时返回对应标识符
	{
		MTimerTaskStatus& task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		task.task_lock.lock();
		task.task_cond.wait(task.task_lock);
		task.task_lock.unlock();
		return true;
	}
	int MTimer::del_task(const Task& task_flag)// 取消定时任务,成功返回对应标识符号，失败返回-1
	{
		Task& ret_task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		ret_task.work_status &= MTimerTaskStatusFlag::FREETASK | MTimerTaskStatusFlag::WORKINGNOW;
		ret_task.activate_times = 0;
		ret_task.wake_times = 0;
		return 0;
	}
	// 获取定时器状态
	inline size_t MTimer::size()// 任务数
	{
		return __MUZI_MTIMER_DATA__->task_size;
	}
	inline size_t MTimer::maxsize()// 最大任务数
	{
		return __MUZI_MTIMER_MAX_TASK_SIZE__;
	}
	inline bool MTimer::is_full()
	{
		return __MUZI_MTIMER_MAX_TASK_SIZE__ == __MUZI_MTIMER_DATA__->task_size;
	}
};