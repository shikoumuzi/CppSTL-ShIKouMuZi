#pragma once
#ifndef __MUZI_MTIMER_H__
#define __MUZI_MTIMER_H__
#include<stdint.h>

namespace MUZI
{
#define __MUZI_MTIMER_MAX_TASK_SIZE__ 1016
#define __MUZI_MTIMER_MAX_THREAD_SIZE__ 8
#define __MUZI_MTIMER_TOTAL_THREAD__ __MUZI_MTIMER_MAX_THREAD_SIZE__ + 1
	using Task = struct MTimerTaskStatus;
	enum TASK
	{
		ONCE_TIME = 0X00000001u,
		MORE_TIMES,
	};

	class MTimer
	{
	public:
		static MTimer* getMTimer();
		static void delMTimer(MTimer* mtimer);
	private:
		MTimer();
		~MTimer();
	public:// ת������
		uint32_t millsecond_2_microsecond(uint32_t microsecond);
		uint32_t second_2_microsecond(uint32_t microsecond);
	public:// ��������
		Task set_task(uint32_t microsecond, int stat);// ���ö�ʱ����,����һ�����
		Task start_task(Task* task_flag);// ��ʼ����
		Task stop_task(Task* task_flag);// ��ͣ����
		Task call_task(const Task* task_flag);// ���ж�ʱ���񣬵���ǰ���񵽹涨����ʱ���ض�Ӧ��ʶ��
		int del_task(Task* task_flag);// ȡ����ʱ����,�ɹ����ض�Ӧ��ʶ���ţ�ʧ�ܷ���-1
	public:// ��ȡ��ʱ��״̬
		size_t size();// ������
		size_t maxsize();// ���������
		bool is_full();
		uint64_t work_time();
	private:
		class MTimerData* data;
	};
};

#endif // !__MUZI_MTIMER_H__

