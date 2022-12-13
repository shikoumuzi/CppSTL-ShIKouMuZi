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
	public:// 转换函数
		uint32_t millsecond_2_microsecond(uint32_t microsecond);
		uint32_t second_2_microsecond(uint32_t microsecond);
	public:// 操作函数
		Task set_task(uint32_t microsecond, int stat);// 设置定时任务,返回一个标号
		Task start_task(Task* task_flag);// 开始任务
		Task stop_task(Task* task_flag);// 暂停任务
		Task call_task(const Task* task_flag);// 呼叫定时任务，当当前任务到规定条件时返回对应标识符
		int del_task(Task* task_flag);// 取消定时任务,成功返回对应标识符号，失败返回-1
	public:// 获取定时器状态
		size_t size();// 任务数
		size_t maxsize();// 最大任务数
		bool is_full();
		uint64_t work_time();
	private:
		class MTimerData* data;
	};
};

#endif // !__MUZI_MTIMER_H__

