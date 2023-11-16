#include"MSignalUtils.h"

namespace MUZI::signal
{
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