#include "MLogicSystem.h"

namespace MUZI::net
{
	LogicSystem::LogicSystem(): m_work_flag(true)
	{
		this->registerCallBacks();

		this->m_work_thread = std::thread(
			[this]()
			{
				ctrlMsg();
			});
	}
	LogicSystem::~LogicSystem()
	{
	}

	void LogicSystem::registerCallBacks()
	{
		this->m_fun_callback[MSG_IDS::MSG_HELLO_WORD] = \
			[](async::NetAsyncIOAdapt, const int msg_id, const std::string& msg_data) {

			};
	}

	void LogicSystem::HelloWordCallBack(async::NetAsyncIOAdapt adapt, const int msg_id, const std::string& msg_data)
	{
		
	}

	void LogicSystem::ctrlMsg()
	{
	}
}