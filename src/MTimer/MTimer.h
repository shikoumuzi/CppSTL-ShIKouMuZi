#pragma once
#ifndef __MUZI_MTIMER_H__
#define __MUZI_MTIMER_H__
#include"../MBase/MObjectBase.h"
#include<stdint.h>
#include<chrono>
namespace MUZI::Timer
{
#define __MUZI_MTIMER_MAX_TASK_SIZE__ 1016
#define __MUZI_MTIMER_MAX_THREAD_SIZE__ 8
#define __MUZI_MTIMER_TOTAL_THREAD__ __MUZI_MTIMER_MAX_THREAD_SIZE__ + 1
#define __MUZI_MTIMER_MIN_TIME_UNIT__ 1 /*������õ���΢����Զ�����Сʱ�䵥λΪ10΢��*/
	using Task = struct MTimerTaskStatus;
	using CTask = const Task&;
	
	class MTimer
	{
	public:
		static Task ErrorTask;
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
		CTask start_task(CTask task_flag);// ��ʼ����
		CTask stop_task(CTask task_flag);// ��ͣ����
		bool call_task(CTask task_flag);// ���ж�ʱ���񣬵���ǰ���񵽹涨����ʱ���ض�Ӧ��ʶ��
		int del_task(CTask task_flag);// ȡ����ʱ����,�ɹ�����0��ʧ�ܷ���-1
	public:// ��ȡ��ʱ��״̬
		size_t size();// ������
		constexpr size_t maxsize();// ���������
		bool is_full();
		uint64_t work_time(CTask task);// ���ؾ����˶���΢��
		bool is_error_task(CTask task);
	public:
		template<typename T1, typename T2 = std::nullptr_t>// ������Ϊ0nullptr_t���͵�ʱ��ȷ��Ϊ1
		uint32_t transition_second(uint32_t orgnal_num)
		{
			// ������if��if constexptr����֧�ֶ�·ԭ��
			// ����Ϊ��ת���������΢���������
			if constexpr (std::is_same<T1, std::nullptr_t>::value)
			{
				if constexpr (std::is_same<T1, T2>::value)
				{
					return orgnal_num;
				}
				else
				{
					if constexpr (std::is_same<T2, std::milli>::value)
					{
						return orgnal_num * 1000;
					}
					else
					{
						if constexpr (std::is_same<T2, std::micro>::value)
						{
							return orgnal_num * 1000000;
						}
					}
				}

			}
			else
			{
				// �����Ǻ��뵽�������ת��
				if constexpr (std::is_same<T1, std::milli>::value)
				{
					if constexpr (std::is_same<T2, std::nullptr_t>::value)
					{
						return orgnal_num / 1000;
					}
					else
					{
						if constexpr (std::is_same<T2, std::micro>::value)
						{
							return orgnal_num * 1000;
						}
					}
				}
				else
				{
					if constexpr (std::is_same<T2, std::nullopt_t>::value)
					{
						return orgnal_num / 1000000;
					}
					else
					{
						if constexpr (std::is_same<T2, std::milli>::value)
						{
							return orgnal_num * 1000;
						}
					}
				}
			}
		}
	private:
		class MTimerData* data;
	};
};

#endif // !__MUZI_MTIMER_H__

