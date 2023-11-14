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
#include<map>
#include<algorithm>
#include<queue>

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
				MSignalUtils::m_sig_cond_var.notify_all(); 

			}
		};
	public:
		
		static void signal_handler(int sig_id)
		{
			// 如果没有目标信号就直接退出
			if (MSignalUtils::m_signals.size() <= 0)
			{
				return;
			}
			// 先检查有无注册信号
			bool element_found_flag = false;

			for (auto x : MSignalUtils::m_signals)
			{
				if (sig_id == x)
				{
					element_found_flag = true;
					break;
				}
			}

			if (element_found_flag == false)
			{
				return;
			}

			MSignalUtils::m_signal_id_queue.push(sig_id);
			MSignalUtils::m_stop_wait_flag = true;
			MSignalUtils::m_sig_cond_var.notify_all();

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
								MSIGNALUTILS_THREAD_LOOP:
									{
										std::unique_lock<std::mutex> unique_lk(MSignalUtils::m_sig_mutex);


										for (auto iter = MSignalUtils::m_funs_before_signal_tigger.begin();
											iter != MSignalUtils::m_funs_before_signal_tigger.end();)
										{
											(*iter)();
											iter = MSignalUtils::m_funs_before_signal_tigger.erase(iter);
										}

										while (!MSignalUtils::m_stop_wait_flag && MSignalUtils::m_signal_id_queue.size() == 0)
										{
											MSignalUtils::m_sig_cond_var.wait(unique_lk);
										}
										MSignalUtils::m_stop_wait_flag = false;

										// 调用信号函数
										auto callback_iter = MSignalUtils::m_funs_when_signal_tigger.find(MSignalUtils::m_signal_id_queue.front());
										if (callback_iter != MSignalUtils::m_funs_when_signal_tigger.end())
										{
											for (auto& x : callback_iter->second)
											{
												x();
											}
										}
										MSignalUtils::m_signal_id_queue.pop();

										if (MSignalUtils::m_thread_loop_work_flag)
										{
											goto MSIGNALUTILS_THREAD_LOOP;
										}
									}


						}));
					//MSignalUtils::m_signal_thread.detach();

					//for (auto x : MSignalUtils::m_signals)
					//{
					//	::signal(x, MSignalUtils::signal_handler);
					//}
				});

		}
	public:
		static void addFunBeforeSignalTrigger(std::function<void()>&& fun)
		{
			std::unique_lock<std::mutex> unique_lk(MSignalUtils::m_sig_mutex);
			MSignalUtils::m_funs_before_signal_tigger.emplace_back(fun);
		}
		static void addFunWhenSignalTrigger(std::function<void()>&& fun, int target_sig_id)
		{
			std::unique_lock<std::mutex> unique_lk(MSignalUtils::m_sig_mutex);
			MSignalUtils::m_funs_when_signal_tigger[target_sig_id].emplace_back(fun);
			MSignalUtils::addSignal(target_sig_id);
		}
		static void addFunWhenSignalTrigger(std::function<void()>&& fun, std::initializer_list<int> target_sig_ids)
		{
			std::unique_lock<std::mutex> unique_lk(MSignalUtils::m_sig_mutex);
			for (auto x : target_sig_ids)
			{
				MSignalUtils::m_funs_when_signal_tigger[x].emplace_back(fun);

			}
			MSignalUtils::addSignals(target_sig_ids);

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
				::signal(x, MSignalUtils::signal_handler);
				MSignalUtils::m_signals.emplace_back(x);
			}
		}

	public:
		static std::vector<std::function<void()>> m_funs_before_signal_tigger;
		static std::map<int, std::vector<std::function<void()>>> m_funs_when_signal_tigger;
		// 信号集合
		static std::queue<int> m_signal_id_queue; // 已经触发的信号队列
		static std::vector<int> m_signals; // 已经注册的信号集合
		// 工作标志
		static std::atomic<bool> m_stop_wait_flag;
		static std::atomic<bool> m_thread_loop_work_flag;
		// 线程
		static std::thread m_signal_thread;
		static std::mutex m_sig_mutex;
		static std::once_flag m_once_init_flag;
		static std::condition_variable m_sig_cond_var;
		static MSignalUtilsCleanUp m_clean_up;
	};

	std::vector<std::function<void()>> MSignalUtils::m_funs_before_signal_tigger;
	std::map<int, std::vector<std::function<void()>>> MSignalUtils::m_funs_when_signal_tigger;

	std::queue<int> MSignalUtils::m_signal_id_queue; // 已经触发的信号队列
	std::vector<int> MSignalUtils::m_signals; // 已经注册的信号集合

	std::atomic<bool> MSignalUtils::m_stop_wait_flag = false;
	std::atomic<bool> MSignalUtils::m_thread_loop_work_flag = true;

	std::thread MSignalUtils::m_signal_thread;
	std::mutex MSignalUtils::m_sig_mutex;
	std::once_flag MSignalUtils::m_once_init_flag;
	std::condition_variable MSignalUtils::m_sig_cond_var;
	MSignalUtils::MSignalUtilsCleanUp MSignalUtils::m_clean_up;



}

#endif // !__MUZI_MSIGNALUTILS_H__

