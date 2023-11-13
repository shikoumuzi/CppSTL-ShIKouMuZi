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
		friend class MSignalUtilsCleanUp;
	private:
		class MSignalUtilsCleanUp
		{
		public:
			MSignalUtilsCleanUp()
			{}
			~MSignalUtilsCleanUp()
			{
				MSignalUtils::m_thread_loop_work_flag = false;
				MSignalUtils::m_stop_wait_flag = true;
			}
		};
	public:
		
		static void signal_handler(int sig_id)
		{
			for (auto x : MSignalUtils::m_signals)
			{
				if (sig_id == x)
				{
					MSignalUtils::m_stop_wait_flag = true;

					for (auto& x : MSignalUtils::m_funs_when_signal_tigger)
					{
						x(sig_id);
					}

					MSignalUtils::m_sig_cond_var.notify_all();
				}
			}
		}
	public:
		static void start()
		{
			std::call_once(MSignalUtils::m_once_init_flag,
				[]() {			
					MSignalUtils::m_signal_thread = \
						std::move(
							std::thread(
								[]()
								{
									while (MSignalUtils::m_thread_loop_work_flag)
									{
										std::unique_lock<std::mutex> unique_lk(MSignalUtils::m_sig_mutex);

										
										for (auto iter = MSignalUtils::m_funs_before_signal_tigger.begin(); 
											iter != MSignalUtils::m_funs_before_signal_tigger.end();)
										{
											(*iter)();
											iter = MSignalUtils::m_funs_before_signal_tigger.erase(iter);
										}

										while (!MSignalUtils::m_stop_wait_flag)
										{
											MSignalUtils::m_sig_cond_var.wait(unique_lk);
										}

										for (auto& x : MSignalUtils::m_funs_after_signal_tigger)
										{
											x();
										}
										 
									}


						}));
					MSignalUtils::m_signal_thread.detach();

					//for (auto x : MSignalUtils::m_signals)
					//{
					//	::signal(x, MSignalUtils::signal_handler);
					//}
				});

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
		static void addFunAfterSignalTrigger(std::function<void()>&& fun)
		{
			MSignalUtils::m_funs_after_signal_tigger.emplace_back(fun);
		}
		static void addSignal(int sig_id)
		{
			::signal(sig_id, MSignalUtils::signal_handler);
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

	private:
		static std::vector<std::function<void()>> m_funs_before_signal_tigger;
		static std::vector<std::function<void(int)>> m_funs_when_signal_tigger;
		static std::vector<std::function<void()>> m_funs_after_signal_tigger;
		static std::vector<int> m_signals;
		static std::atomic<bool> m_stop_wait_flag;
		static std::atomic<bool> m_thread_loop_work_flag;
		static std::atomic<int> m_sig_id;
		static std::thread m_signal_thread;
		static std::mutex m_sig_mutex;
		static std::once_flag m_once_init_flag;
		static std::condition_variable m_sig_cond_var;
		static MSignalUtilsCleanUp m_clean_up;
	};

	std::atomic<bool> MSignalUtils::m_stop_wait_flag = false;
	std::atomic<bool> MSignalUtils::m_thread_loop_work_flag = true;

}

#endif // !__MUZI_MSIGNALUTILS_H__
