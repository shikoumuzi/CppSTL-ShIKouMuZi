#pragma once
#ifndef __MUZI_MEVENTLOOP_H__
#define __MUZI_MEVENTLOOP_H__
#include"MEvent.h"
#include<memory>
#include<thread>
#include<queue>
#include<mutex>
#include"MSTL/h/MAtomicLock.h"
namespace MUZI::_event
{
	class MEventLoop
	{
	public:
		using MEventQueue = std::priority_queue<MEvent>;
		using MThreadId = std::thread::id;
		using MRMutex = std::recursive_mutex;
		using MTime = std::chrono::system_clock::time_point;

	public:
		MEventLoop();
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
		bool m_atomic_mode;
		bool m_work_flag;

	};
}
#endif // !__MUZI_MEVENTLOOP_H__
