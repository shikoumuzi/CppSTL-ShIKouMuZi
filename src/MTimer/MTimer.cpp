#include"MTimer.h"
#include<thread>
#include<chrono>
#include<mutex>
#include<atomic>
#include<stdint.h>
#include<errno.h>

namespace MUZI::Timer
{
	// 数据结构
	static class MTimerData* __MUZI_MTIMER_DATA__ = nullptr;
	static std::mutex __MUZI_MTIMER_INIT_LOCK__;
	using __MUZI_MTIMER_MIN_TIME_UNIT_TYPE__ = std::chrono::milliseconds;// 最小时间单位类型
	using __MUZI_MTIMER_MIN_TIME_UNIT_TYPE_RATIONAL__ = std::milli;// 最小时间单位对应的有理数

	class MTimerData
	{
	public:
		static MTimerData* getMTimerData();
		static void delMTimerData(MTimerData** data);
	public:
		static Task& get_free_task();
		static bool add_task(const Task& task);

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
		WORKINGNOW = 0x00000001u,// 代表该人物处于非工作状态（暂停或者未启动）
		NOWORKINGNOW = 0x00000010u,// 代表该任务处于工作状态
		FREETASK = 0x10000000u,// 代表该任务目前处于没有接手的状态
		BUSYTASK = 0x01000000u,// 代表该任务处于接手状态
		ERRORTASK = 0x00011000u
	};

	struct MTimerTaskStatus
	{
		std::atomic<size_t> work_status;
		std::chrono::steady_clock::time_point task_start_time;// 任务起始时间
		std::atomic<uint64_t> wake_times;// 被唤醒次数
		std::atomic<uint64_t> activate_times;// 激活所需要的次数
		std::mutex contributing_lock;// 起作用的 
		//std::unique_lock<std::mutex> task_lock;// 条件变量匹配的锁, 用以传参, 同时该unqiue_lock一旦初始化，立刻上锁
		std::condition_variable task_cond;// 激活的条件变量，用以激活call_task函数
		size_t pos;
		size_t thread_pos;
	};

	// 定时线程
	struct MTimerChildThread
	{
		std::atomic<size_t> min_pos;// 负责范围内的最低值
		std::atomic<size_t> max_pos;// 负责范围内的最高值
		std::thread child_th;
		// 下面是让main_thread唤醒子线程所需要的函数
		std::condition_variable* cond;
		std::mutex* cond_lock;
		size_t tasks_num;
	};
	void __MUZI_MTIMER_CHILD_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerChildThread& child_thread = *static_cast<struct MTimerChildThread*>(job_ptr);
		std::unique_lock<std::mutex> cond_lock(*child_thread.cond_lock);
		while (!__MUZI_MTIMER_DATA__->exit_flag)
		{
			// 子线程 处于等待状态，等待主线程抵达下一时间单元的命令
			child_thread.cond->wait(cond_lock);
			for (size_t i = child_thread.min_pos; i <= child_thread.max_pos; ++i)
			{
				if (__MUZI_MTIMER_DATA__->tasks_status[i].work_status & MTimerTaskStatusFlag::WORKINGNOW)
				{
					struct MTimerTaskStatus& task_data = __MUZI_MTIMER_DATA__->tasks_status[i];
					++task_data.wake_times;
					if ((task_data.wake_times % task_data.activate_times) <= (__MUZI_MTIMER_MIN_TIME_UNIT__))
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
		std::thread main_th;// 计时线程
		std::condition_variable cond;// 用以传递计时信息的条件变量
		std::mutex contributing_lock;// 起作用的锁
		//std::unique_lock<std::mutex> cond_unique_lock;
		//// std::unique_lock实际上是封装mutex，所以在初始化的时候必须要导入一个std::mutex进行初始化，否则会报死锁错误
		////同时该unqiue_lock一旦初始化，立刻上锁
		std::chrono::steady_clock::time_point work_start_time;// 线程工作时间
		__MUZI_MTIMER_MIN_TIME_UNIT_TYPE__ timeout_standnrd;// 最小时间单元
	};
	void __MUZI_MTIMER_MAIN_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerMainThread& main_thread = *static_cast<struct MTimerMainThread*>(job_ptr);
		main_thread.work_start_time = std::chrono::steady_clock::now();
		static std::unique_lock<std::mutex> main_thread_lock(main_thread.contributing_lock);
		// 根据最小单位叠加，等到抵达下一个单元的时候向其他线程提醒动作
#ifdef WINDOWS
		while (((main_thread.cond.
			wait_for(main_thread_lock, main_thread.timeout_standnrd) == std::cv_status::timeout)
			|| ::GetLastError() == ETIMEDOUT) && (!__MUZI_MTIMER_DATA__->exit_flag))
		{
			main_thread.cond.notify_all();
		}
#elif define LINUX
		while (((main_thread.cond.
			wait_for(main_thread_lock, main_thread.timeout_standnrd) == std::cv_status::timeout)) 
			&& (!__MUZI_MTIMER_DATA__->exit_flag))
		{
			main_thread.cond.notify_all();
		}

#endif // WINDOWS


	}

	// 静态变量初始化和 函数定义
	struct MTimerTaskStatus MTimerData::tasks_status[__MUZI_MTIMER_MAX_TASK_SIZE__] = {};
	struct MTimerChildThread MTimerData::tasks_threads[__MUZI_MTIMER_MAX_THREAD_SIZE__] = {};
	struct MTimerMainThread MTimerData::main_thread;
	bool MTimerData::exit_flag = false;
	size_t MTimerData::task_size = 0;
	size_t MTimerData::object_num = 0;
	Task MTimer::ErrorTask;


	MTimerData* MTimerData::getMTimerData()
	{
		__MUZI_MTIMER_INIT_LOCK__.lock();
		if (__MUZI_MTIMER_DATA__ == nullptr)
		{
			__MUZI_MTIMER_DATA__ = new MTimerData;
			size_t i = 0;
			MTimerTaskStatus* tasks_status = nullptr;
			// 初始化任务
			for (; i < __MUZI_MTIMER_MAX_TASK_SIZE__; ++i)
			{
				tasks_status = &__MUZI_MTIMER_DATA__->tasks_status[i];
				tasks_status->work_status = MTimerTaskStatusFlag::FREETASK;
				tasks_status->wake_times = 0;
				tasks_status->activate_times = 0;
				tasks_status->pos = i;
			}
			// 初始化主线程
			static struct MTimerMainThread& main_thread = __MUZI_MTIMER_DATA__->main_thread;

			// 设定任务开始时间
			main_thread.work_start_time = std::chrono::steady_clock::now();
			// 初始化为__MUZI_MTIMER_MIN_TIME_UNIT__作为一个时间单位
			//main_thread.timeout_standnrd = std::chrono::duration<uint32_t, __MUZI_MTIMER_MIN_TIME_UNIT_TYPE_RATIONAL__>(__MUZI_MTIMER_MIN_TIME_UNIT__);
			main_thread.timeout_standnrd = std::chrono::milliseconds(__MUZI_MTIMER_MIN_TIME_UNIT__);
			main_thread.main_th = std::move(std::thread(__MUZI_MTIMER_MAIN_THREAD_FUN__, &__MUZI_MTIMER_DATA__->main_thread));

			// 初始化子线程
			struct MTimerChildThread* child_threads = __MUZI_MTIMER_DATA__->tasks_threads;
			size_t thread_tasks_num = __MUZI_MTIMER_MAX_TASK_SIZE__ / __MUZI_MTIMER_MAX_THREAD_SIZE__;
			size_t start_pos = 0, end_pos = thread_tasks_num;
			size_t j = 0;
			for (i = 0; i < __MUZI_MTIMER_MAX_THREAD_SIZE__; ++i)
			{
				child_threads[i].cond = &main_thread.cond;
				//child_threads[i].cond_unique_lock = &main_thread.cond_unique_lock;
				child_threads[i].cond_lock = &main_thread.contributing_lock;
				child_threads[i].min_pos = start_pos;
				child_threads[i].max_pos = end_pos;
				child_threads[i].tasks_num = 0;
				//child_threads[i].child_th = std::move(std::thread(__MUZI_MTIMER_CHILD_THREAD_FUN__, &child_threads[i]));
				for (j = start_pos; j <= end_pos; ++j)
				{
					__MUZI_MTIMER_DATA__->tasks_status[i].thread_pos = i;
				}
				start_pos = end_pos + 1;
				end_pos += thread_tasks_num;
			}
		}
		__MUZI_MTIMER_INIT_LOCK__.unlock();
		return __MUZI_MTIMER_DATA__;

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
		return MTimer::ErrorTask;
	}
	// 给线程添加任务，添加该函数的目的是保证没有进行任务的线程不会被创建，只有当有任务的线程才会启动
	bool MTimerData::add_task(const Task& task_flag)
	{
		Task& task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		if (task.work_status & MTimerTaskStatusFlag::WORKINGNOW)// 首先查看是否处于工作状态，确保不重复对一个任务进行操作
		{
			return false;
		}
		// 确定是一个未工作中的线程
		MTimerChildThread& tasks_thread = __MUZI_MTIMER_DATA__->tasks_threads[task.thread_pos];
		task.work_status = MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW;
		task.task_start_time = std::chrono::steady_clock::now();
		if (tasks_thread.tasks_num == 0)// 如果挂载在该线程上的任务为0，则代表该线程没有人使用过，所以需要创立线程
		{
			tasks_thread.child_th = std::move(std::thread(__MUZI_MTIMER_CHILD_THREAD_FUN__, &tasks_thread));
		}
		++tasks_thread.tasks_num;
		return true;
	}

	// 下面是头文件所看到的函数内容
	MTimer* MTimer::getMTimer()
	{
		MTimer::ErrorTask.work_status = MTimerTaskStatusFlag::ERRORTASK;
		MTimer::ErrorTask.wake_times = static_cast<uint64_t>(-1);
		MTimer::ErrorTask.activate_times = static_cast<uint64_t>(-1);
		MTimer::ErrorTask.pos = static_cast<size_t>(-1);
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

	// 操作函数
	const Task& MTimer::set_task(uint32_t milisecond)// 设置定时任务,返回一个标号
	{
		Task& ret_task = this->data->get_free_task();
		if (!this->is_error_task(ret_task))
		{
			ret_task.activate_times = milisecond / __MUZI_MTIMER_MIN_TIME_UNIT__;
			ret_task.work_status = MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::NOWORKINGNOW;
			__MUZI_MTIMER_DATA__->task_size += 1;
		}
		return ret_task;
	}
	const Task& MTimer::start_task(const Task& task_flag)// 开始任务
	{
		//Task& ret_task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		//ret_task.work_status &= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW;
		//return ret_task;
		return (__MUZI_MTIMER_DATA__->add_task(task_flag), task_flag);
	}
	const Task& MTimer::stop_task(const Task& task_flag)// 暂停任务
	{
		return (__MUZI_MTIMER_DATA__->tasks_status[task_flag.pos].work_status
			&= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::NOWORKINGNOW, task_flag);
	}
	bool MTimer::call_task(const Task& task_flag)// 呼叫定时任务，当当前任务到规定条件时返回对应标识符
	{
		MTimerTaskStatus& task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		std::unique_lock<std::mutex> task_lock(task.contributing_lock);
		task.task_cond.wait(task_lock);
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
	constexpr size_t MTimer::maxsize()// 最大任务数
	{
		return __MUZI_MTIMER_MAX_TASK_SIZE__;
	}
	inline bool MTimer::is_full()
	{
		return __MUZI_MTIMER_MAX_TASK_SIZE__ == __MUZI_MTIMER_DATA__->task_size;
	}
	uint64_t MTimer::work_time(const Task& task)
	{
		return std::chrono::duration_cast<__MUZI_MTIMER_MIN_TIME_UNIT_TYPE__>(std::chrono::steady_clock::now() - task.task_start_time).count();
	}
	inline bool MTimer::is_error_task(const Task& task)
	{
		return task.work_status & MTimerTaskStatusFlag::ERRORTASK;
	}
};