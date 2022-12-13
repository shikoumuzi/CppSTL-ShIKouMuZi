#include"MTimer.h"
#include<thread>
#include<chrono>
#include<mutex>
#include<atomic>
#include<stdint.h>
#include<errno.h>

namespace MUZI
{
	// ���ݽṹ
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
	// ����״̬
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
		std::chrono::steady_clock::time_point task_start_time;// ������ʼʱ��
		std::atomic<uint64_t> wake_times;// �����Ѵ���
		std::atomic<uint64_t> activate_times;// ��������Ҫ�Ĵ���
		std::unique_lock<std::mutex> task_lock;// ��������ƥ�����
		std::condition_variable task_cond;// ������������������Լ���call_task����
		size_t pos;
	};

	// ��ʱ�߳�
	struct MTimerChildThread
	{
		std::atomic<size_t> min_pos;// ����Χ�ڵ����ֵ
		std::atomic<size_t> max_pos;// ����Χ�ڵ����ֵ
		std::thread child_th;
		// ��������main_thread�������߳�����Ҫ�ĺ���
		std::condition_variable* cond;
		std::unique_lock<std::mutex>* cond_unique_lock;
	};
	void __MUZI_MTIMER_CHILD_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerChildThread& child_thread = *static_cast<struct MTimerChildThread*>(job_ptr);
		while (!__MUZI_MTIMER_DATA__->exit_flag)
		{
			// ���߳� ���ڵȴ�״̬���ȴ����̵ִ߳���һʱ�䵥Ԫ������
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


	// ��ʱ�߳�
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
		// ������С��λ���ӣ��ȵ��ִ���һ����Ԫ��ʱ���������߳����Ѷ���
		while (((main_thread.cond.
			wait_for(main_thread.cond_unique_lock, main_thread.timeout_standnrd) == std::cv_status::timeout)
			|| errno == ETIMEDOUT) && (!__MUZI_MTIMER_DATA__->exit_flag))
		{
			main_thread.cond.notify_all();
		}
	}

	// ��̬������ʼ���� ��������
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
			// ��ʼ������
			for (; i < __MUZI_MTIMER_MAX_TASK_SIZE__; ++i)
			{
				__MUZI_MTIMER_DATA__->tasks_status[i].work_status = MTimerTaskStatusFlag::FREETASK;
				__MUZI_MTIMER_DATA__->tasks_status[i].wake_times = 0;
				__MUZI_MTIMER_DATA__->tasks_status[i].activate_times = 0;
				__MUZI_MTIMER_DATA__->tasks_status[i].pos = i;
			}
			// ��ʼ�����߳�
			static struct MTimerMainThread& main_thread = __MUZI_MTIMER_DATA__->main_thread;

			// �趨����ʼʱ��
			main_thread.work_start_time = std::chrono::steady_clock::now();
			// ��ʼ��Ϊ10΢����Ϊһ��ʱ�䵥λ
			main_thread.timeout_standnrd = std::chrono::duration<uint64_t, std::micro>(10);
			main_thread.main_th = std::move(std::thread(__MUZI_MTIMER_MAIN_THREAD_FUN__, &__MUZI_MTIMER_DATA__->main_thread));

			// ��ʼ�����߳�
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


	// ������ͷ�ļ��������ĺ�������
	MTimer* MTimer::getMTimer()
	{
		return new MTimer();
	}
	void MTimer::delMTimer(MTimer** mtimer)
	{
		delete* mtimer;
		*mtimer = nullptr;
	}
	//��ʼ������
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
	// ת������
	inline uint32_t MTimer::millsecond_2_microsecond(uint32_t millsecond)
	{
		return static_cast<uint32_t>(millsecond / 1000);
	}
	inline uint32_t MTimer::second_2_microsecond(uint32_t second)
	{
		return static_cast<uint32_t>(second / 1000000);
	}
	// ��������
	const Task& MTimer::set_task(uint32_t microsecond)// ���ö�ʱ����,����һ�����
	{
		Task& ret_task = this->data->get_free_task();
		ret_task.activate_times = microsecond / __MUZI_MTIMER_MIN_TIME_UNIT__;
		ret_task.work_status &= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::NOWORKINGNOW;
		__MUZI_MTIMER_DATA__->task_size += 1;
		return ret_task;
	}
	inline const Task& MTimer::start_task(const Task& task_flag)// ��ʼ����
	{
		//Task& ret_task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		//ret_task.work_status &= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW;
		//return ret_task;
		return (__MUZI_MTIMER_DATA__->tasks_status[task_flag.pos].work_status
			&= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW, task_flag);
	}
	const Task& MTimer::stop_task(const Task& task_flag)// ��ͣ����
	{
		return (__MUZI_MTIMER_DATA__->tasks_status[task_flag.pos].work_status
			&= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::NOWORKINGNOW, task_flag);
	}
	bool MTimer::call_task(const Task& task_flag)// ���ж�ʱ���񣬵���ǰ���񵽹涨����ʱ���ض�Ӧ��ʶ��
	{
		MTimerTaskStatus& task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		task.task_lock.lock();
		task.task_cond.wait(task.task_lock);
		task.task_lock.unlock();
		return true;
	}
	int MTimer::del_task(const Task& task_flag)// ȡ����ʱ����,�ɹ����ض�Ӧ��ʶ���ţ�ʧ�ܷ���-1
	{
		Task& ret_task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		ret_task.work_status &= MTimerTaskStatusFlag::FREETASK | MTimerTaskStatusFlag::WORKINGNOW;
		ret_task.activate_times = 0;
		ret_task.wake_times = 0;
		return 0;
	}
	// ��ȡ��ʱ��״̬
	inline size_t MTimer::size()// ������
	{
		return __MUZI_MTIMER_DATA__->task_size;
	}
	inline size_t MTimer::maxsize()// ���������
	{
		return __MUZI_MTIMER_MAX_TASK_SIZE__;
	}
	inline bool MTimer::is_full()
	{
		return __MUZI_MTIMER_MAX_TASK_SIZE__ == __MUZI_MTIMER_DATA__->task_size;
	}
};