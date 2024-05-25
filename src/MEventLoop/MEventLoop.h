#pragma once
#ifndef __MUZI_MEVENTLOOP_H__
#define __MUZI_MEVENTLOOP_H__
#include"MEvent.h"
#include<memory>
#include<thread>
#include<queue>
#include<mutex>
#include"MSTL/include/MAtomicLock.h"
#include"MSTL/include/MSkipList.h"
#include<atomic>

#define __MUZI_MEVENTLOOP_DEFAULT_SLEEP_TIME_IN_MILLISECOND_FOR_ENDLESS_LOOP__ std::this_thread::sleep_for(std::chrono::milliseconds(10))
namespace MUZI::_event
{
	class MEventLoop
	{
	public:
		// 这个可以用跳表代替
		using MEventQueue = std::priority_queue<MEvent>;
		using MThreadId = std::thread::id;
		using MRMutex = std::recursive_mutex;
		using MTime = std::chrono::system_clock::time_point;
		using EventCtrlCallBack = void(*)(const MEvent* event);
	public:
		MEventLoop(EventCtrlCallBack&& event_ctrl_callback);
	public:
		void push_back(const MEvent& event);
		void run();
	private:
		void ctrlEvent(const MEvent& event);
		void analyizeCtrlMode();
	private:
		MThreadId m_thread_id;
		MEventQueue m_event_queue;
		MRMutex m_rmutex;
		MAtomicLock m_amutex;
		std::atomic<bool> m_atomic_mode;
		std::atomic<bool> m_work_flag;
		EventCtrlCallBack m_event_ctrl_callback;
	};
}
#endif // !__MUZI_MEVENTLOOP_H__
