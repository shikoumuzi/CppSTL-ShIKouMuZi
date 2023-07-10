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
#define __MUZI_MTIMER_MIN_TIME_UNIT__ 1 /*这里采用的是微妙，所以定义最小时间单位为10微妙*/
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
	public:// 转换函数
		uint32_t millsecond_2_microsecond(uint32_t millsecond);
		uint32_t second_2_microsecond(uint32_t second);
	public:// 操作函数
		CTask set_task(uint32_t microsecond);// 设置定时任务,返回一个标号
		CTask start_task(CTask task_flag);// 开始任务
		CTask stop_task(CTask task_flag);// 暂停任务
		bool call_task(CTask task_flag);// 呼叫定时任务，当当前任务到规定条件时返回对应标识符
		int del_task(CTask task_flag);// 取消定时任务,成功返回0，失败返回-1
	public:// 获取定时器状态
		size_t size();// 任务数
		constexpr size_t maxsize();// 最大任务数
		bool is_full();
		uint64_t work_time(CTask task);// 返回经历了多少微妙
		bool is_error_task(CTask task);
	public:
		template<typename T1, typename T2 = std::nullptr_t>// 当输入为0nullptr_t类型的时候确认为1
		uint32_t transition_second(uint32_t orgnal_num)
		{
			// 编译器if（if constexptr）不支持短路原则
			// 首先为秒转换到毫秒和微秒的数量级
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
				// 这里是毫秒到其他妙的转换
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

