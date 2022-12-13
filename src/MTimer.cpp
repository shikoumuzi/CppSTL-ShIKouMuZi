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
	class MTimerData* __MUZI_MTIMER_DATA__ = nullptr;
	static std::mutex* __MUZI_MTIMER_INIT_LOCK__ = new std::mutex;
	// ����״̬
	struct MTimerTaskStatus
	{
		uint64_t cycle_times;// ʱ��������
	};

	// ��ʱ�߳�
	using ChildThreadFunction = void(*)(void*);
	struct MTimerChildThread
	{
		size_t min_pos;// ����Χ�ڵ����ֵ
		size_t max_pos;// ����Χ�ڵ����ֵ
		std::thread child_th;
		std::condition_variable* cond;
		std::unique_lock<std::mutex>* cond_unique_lock;
	};
	void __MUZI_MTIMER_CHILD_THREAD_FUN__(void* job_ptr)
	{
		struct MTimerChildThread& child_thread = *static_cast<struct MTimerChildThread*>(job_ptr);
		while (1)
		{
			// ���߳� ���ڵȴ�״̬���ȴ����̵ִ߳���һʱ�䵥Ԫ������
			child_thread.cond->wait(*child_thread.cond_unique_lock);
			for (size_t i = child_thread.min_pos; i <= child_thread.max_pos; ++i)
			{
				// do something
				__MUZI_MTIMER_DATA__->tasks_status[i];
			}
		}
	}


	// ��ʱ�߳�
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
		// ������С��λ���ӣ��ȵ��ִ���һ����Ԫ��ʱ���������߳����Ѷ���
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
				// ��ʼ������
				for (; i < __MUZI_MTIMER_MAX_TASK_SIZE__; ++i)
				{

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
//��ʼ������
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