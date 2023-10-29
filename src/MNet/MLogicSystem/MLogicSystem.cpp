#include "MLogicSystem.h"
#include<cstring>
namespace MUZI::net
{
	LogicSystem::LogicSystem(async::MAsyncServer& server): m_work_flag(true), m_server(server)
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
		rapidjson::Document doc;
		if (!doc.Parse(msg_data.c_str()).HasParseError())
		{
			if (doc.HasMember("id") && doc.IsInt())
			{
				printf("msg_id is %d,", doc["id"].GetInt());
			}
			if (doc.HasMember("data") && doc.IsString())
			{
				printf("data is %s.", doc["data"].GetString());
			}
		}
		
		this->m_server.writePackage(adapt, msg_data);

	}

	void LogicSystem::ctrlMsg()
	{
	}
}