#pragma once
#ifndef __MUZI_MSIGNALUTILS_H__
#define __MUZI_MSIGNALUTILS_H__
#include"MSingleton/MSingleton.h"
#include<signal.h>
#include<functional>
#include<vector>
#include<thread>
#include<mutex>
#include<atomic>
namespace MUZI::signal
{
	class MSignalUtils: public singleton::MSingleton<MSignalUtils>
	{
	public:
		
		static void signal_handler(int sig_id)
		{
			if (sig_id == SIGINT || sig_id == SIGTERM)
			{
				MSignalUtils::m_bstop = true;

			}
		}
	public:
		MSignalUtils()
		{
			
		}
	public:
		static void addFunBeforeSignalTrigger(std::function<void()>&& fun)
		{
			MSignalUtils::m_funs_before_signal_tigger.emplace_back(fun);
		}
		static void addFunWhenSignalTrigger(std::function<void(int)>&& fun)
		{
			MSignalUtils::m_funs_when_signal_tigger.emplace_back(fun);
		}
		static void addFunAfterSignalTrigger(std::function<void(int)>&& fun)
		{
			MSignalUtils::m_funs_after_signal_tigger.emplace_back(fun);
		}
		static void addSignal(int sig_id)
		{
			MSignalUtils::m_signals.emplace_back(sig_id);
		}
		static void addSignals(std::initializer_list<int> signals)
		{
			MSignalUtils::m_signals.reserve(MSignalUtils::m_signals.size() + signals.size());
			for (auto x : signals)
			{
				MSignalUtils::m_signals.emplace_back(x);
			}
		}

	public:
		static void start()
		{

		}
	public:
		static std::vector<std::function<void()>> m_funs_before_signal_tigger;
		static std::vector<std::function<void()>> m_funs_when_signal_tigger;
		static std::vector<std::function<void()>> m_funs_after_signal_tigger;
		static std::vector<int> m_signals;
		static std::atomic<bool> m_bstop;
		static std::thread m_signal_thread;
		static std::mutex m_sig_mutex;
		static std::condition_variable m_sig_cond_var;

	};
}

#endif // !__MUZI_MSIGNALUTILS_H__

