#include"MTimer.h"
#include<thread>
#include<chrono>
#include<mutex>
#include<atomic>
#include<stdint.h>
#include<errno.h>

namespace MUZI::Timer
{
	// ���ݽṹ
	static class MTimerData* __MUZI_MTIMER_DATA__ = nullptr;
	static std::mutex __MUZI_MTIMER_INIT_LOCK__;
	using __MUZI_MTIMER_MIN_TIME_UNIT_TYPE__ = std::chrono::milliseconds;// ��Сʱ�䵥λ����
	using __MUZI_MTIMER_MIN_TIME_UNIT_TYPE_RATIONAL__ = std::milli;// ��Сʱ�䵥λ��Ӧ��������

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
	// ����״̬
	enum MTimerTaskStatusFlag
	{
		WORKINGNOW = 0x00000001u,// ��������ﴦ�ڷǹ���״̬����ͣ����δ������
		NOWORKINGNOW = 0x00000010u,// ����������ڹ���״̬
		FREETASK = 0x10000000u,// ���������Ŀǰ����û�н��ֵ�״̬
		BUSYTASK = 0x01000000u,// ����������ڽ���״̬
		ERRORTASK = 0x00011000u
	};

	struct MTimerTaskStatus
	{
		std::atomic<size_t> work_status;
		std::chrono::steady_clock::time_point task_start_time;// ������ʼʱ��
		std::atomic<uint64_t> wake_times;// �����Ѵ���
		std::atomic<uint64_t> activate_times;// ��������Ҫ�Ĵ���
		std::mutex contributing_lock;// �����õ� 
		//std::unique_lock<std::mutex> task_lock;// ��������ƥ�����, ���Դ���, ͬʱ��unqiue_lockһ����ʼ������������
		std::condition_variable task_cond;// ������������������Լ���call_task����
		size_t pos;
		size_t thread_pos;
	};

	// ��ʱ�߳�
	struct MTimerChildThread
	{
		std::atomic<size_t> min_pos;// ����Χ�ڵ����ֵ
		std::atomic<size_t> max_pos;// ����Χ�ڵ����ֵ
		std::thread child_th;
		// ��������main_thread�������߳�����Ҫ�ĺ���
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
			// ���߳� ���ڵȴ�״̬���ȴ����̵ִ߳���һʱ�䵥Ԫ������
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


	// ��ʱ�߳�
	struct MTimerMainThread
	{
		std::thread main_th;// ��ʱ�߳�
		std::condition_variable cond;// ���Դ��ݼ�ʱ��Ϣ����������
		std::mutex contributing_lock;// �����õ���
		//std::unique_lock<std::mutex> cond_unique_lock;
		//// std::unique_lockʵ�����Ƿ�װmutex�������ڳ�ʼ����ʱ�����Ҫ����һ��std::mutex���г�ʼ��������ᱨ��������
		////ͬʱ��unqiue_lockһ����ʼ������������
		std::chrono::steady_clock::time_point work_start_time;// �̹߳���ʱ��
		__MUZI_MTIMER_MIN_TIME_UNIT_TYPE__ timeout_standnrd;// ��Сʱ�䵥Ԫ
	};
	void __MUZI_MTIMER_MAIN_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerMainThread& main_thread = *static_cast<struct MTimerMainThread*>(job_ptr);
		main_thread.work_start_time = std::chrono::steady_clock::now();
		static std::unique_lock<std::mutex> main_thread_lock(main_thread.contributing_lock);
		// ������С��λ���ӣ��ȵ��ִ���һ����Ԫ��ʱ���������߳����Ѷ���
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

	// ��̬������ʼ���� ��������
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
			// ��ʼ������
			for (; i < __MUZI_MTIMER_MAX_TASK_SIZE__; ++i)
			{
				tasks_status = &__MUZI_MTIMER_DATA__->tasks_status[i];
				tasks_status->work_status = MTimerTaskStatusFlag::FREETASK;
				tasks_status->wake_times = 0;
				tasks_status->activate_times = 0;
				tasks_status->pos = i;
			}
			// ��ʼ�����߳�
			static struct MTimerMainThread& main_thread = __MUZI_MTIMER_DATA__->main_thread;

			// �趨����ʼʱ��
			main_thread.work_start_time = std::chrono::steady_clock::now();
			// ��ʼ��Ϊ__MUZI_MTIMER_MIN_TIME_UNIT__��Ϊһ��ʱ�䵥λ
			//main_thread.timeout_standnrd = std::chrono::duration<uint32_t, __MUZI_MTIMER_MIN_TIME_UNIT_TYPE_RATIONAL__>(__MUZI_MTIMER_MIN_TIME_UNIT__);
			main_thread.timeout_standnrd = std::chrono::milliseconds(__MUZI_MTIMER_MIN_TIME_UNIT__);
			main_thread.main_th = std::move(std::thread(__MUZI_MTIMER_MAIN_THREAD_FUN__, &__MUZI_MTIMER_DATA__->main_thread));

			// ��ʼ�����߳�
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
	// ���߳����������Ӹú�����Ŀ���Ǳ�֤û�н���������̲߳��ᱻ������ֻ�е���������̲߳Ż�����
	bool MTimerData::add_task(const Task& task_flag)
	{
		Task& task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		if (task.work_status & MTimerTaskStatusFlag::WORKINGNOW)// ���Ȳ鿴�Ƿ��ڹ���״̬��ȷ�����ظ���һ��������в���
		{
			return false;
		}
		// ȷ����һ��δ�����е��߳�
		MTimerChildThread& tasks_thread = __MUZI_MTIMER_DATA__->tasks_threads[task.thread_pos];
		task.work_status = MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW;
		task.task_start_time = std::chrono::steady_clock::now();
		if (tasks_thread.tasks_num == 0)// ��������ڸ��߳��ϵ�����Ϊ0���������߳�û����ʹ�ù���������Ҫ�����߳�
		{
			tasks_thread.child_th = std::move(std::thread(__MUZI_MTIMER_CHILD_THREAD_FUN__, &tasks_thread));
		}
		++tasks_thread.tasks_num;
		return true;
	}

	// ������ͷ�ļ��������ĺ�������
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

	// ��������
	const Task& MTimer::set_task(uint32_t milisecond)// ���ö�ʱ����,����һ�����
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
	const Task& MTimer::start_task(const Task& task_flag)// ��ʼ����
	{
		//Task& ret_task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		//ret_task.work_status &= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::WORKINGNOW;
		//return ret_task;
		return (__MUZI_MTIMER_DATA__->add_task(task_flag), task_flag);
	}
	const Task& MTimer::stop_task(const Task& task_flag)// ��ͣ����
	{
		return (__MUZI_MTIMER_DATA__->tasks_status[task_flag.pos].work_status
			&= MTimerTaskStatusFlag::BUSYTASK | MTimerTaskStatusFlag::NOWORKINGNOW, task_flag);
	}
	bool MTimer::call_task(const Task& task_flag)// ���ж�ʱ���񣬵���ǰ���񵽹涨����ʱ���ض�Ӧ��ʶ��
	{
		MTimerTaskStatus& task = __MUZI_MTIMER_DATA__->tasks_status[task_flag.pos];
		std::unique_lock<std::mutex> task_lock(task.contributing_lock);
		task.task_cond.wait(task_lock);
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
	constexpr size_t MTimer::maxsize()// ���������
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