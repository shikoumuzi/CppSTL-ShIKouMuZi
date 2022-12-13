#pragma once
#ifndef __MUZI_MTIMER_H__
#define __MUZI_MTIMER_H__
#include<stdint.h>

namespace MUZI
{
#define __MUZI_MTIMER_MAX_TASK_SIZE__ 1016
#define __MUZI_MTIMER_MAX_THREAD_SIZE__ 8
#define __MUZI_MTIMER_TOTAL_THREAD__ __MUZI_MTIMER_MAX_THREAD_SIZE__ + 1
#define __MUZI_MTIMER_MIN_TIME_UNIT__ 10 /*������õ���΢����Զ�����Сʱ�䵥λΪ10΢��*/
	using Task = struct MTimerTaskStatus;
	using CTask = const Task&;
	class MTimer
	{
	public:
		using value_type = class MTimerData*;
	public:
		static MTimer* getMTimer();
		static void delMTimer(MTimer** mtimer);
	private:
		MTimer();
		MTimer(const MTimer&) = delete;
		MTimer(MTimer&&) = delete;
		~MTimer();
	public:// ת������
		uint32_t millsecond_2_microsecond(uint32_t millsecond);
		uint32_t second_2_microsecond(uint32_t second);
	public:// ��������
		CTask set_task(uint32_t microsecond);// ���ö�ʱ����,����һ�����
		CTask start_task(const Task& task_flag);// ��ʼ����
		CTask stop_task(const Task& task_flag);// ��ͣ����
		bool call_task(const Task& task_flag);// ���ж�ʱ���񣬵���ǰ���񵽹涨����ʱ���ض�Ӧ��ʶ��
		int del_task(const Task& task_flag);// ȡ����ʱ����,�ɹ�����0��ʧ�ܷ���-1
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

