#include"MFTP.h"
#include"../MBase/MError.h"
namespace MUZI
{
	// 定义
	enum MFTP::ProtoType
	{
		IDENTIFICATION = 1,
		DATA,
		END

	};
	enum MFTP::ProtoID
	{
		MFTP_BASE_MESSAGE = -1,
		MFTP_IDENTIFICATION,
	};


	// 构造函数
	MFTP::MFTP()
		:identification({ BaseMessage(ProtoID::MFTP_BASE_MESSAGE, sizeof(Identification), ProtoType::IDENTIFICATION, 0), 0, 0, 0, 0, 0 }),
		role(ROLE_ID::UNKOWN),
		root_file_directory(nullptr) {}

	MFTP::MFTP(ROLE role, uint16_t client_ip4_addr, unsigned int client_port, uint16_t server_ip4_addr, unsigned int server_port)
		:identification({ BaseMessage(ProtoID::MFTP_BASE_MESSAGE, sizeof(Identification), ProtoType::IDENTIFICATION, 0),
				client_ip4_addr, server_ip4_addr, client_port, server_port, 0 }),
		role(ROLE_ID::UNKOWN),
		root_file_directory(nullptr){}

	MFTP::MFTP(MFTP&& that)noexcept
		:identification(that.identification),
		role(that.role),
		root_file_directory(that.root_file_directory){}

	//连接函数
	int MFTP::searchEachOther()
	{
		if (this->role == UNKOWN)
		{
			return MERROR::UNUSEFUL;
		}
		return 0;
	}
	int MFTP::connect()
	{
		Identification identification;
		BaseMessage& bm = identification.base_message;
		memcpy(&identification, &(this->identification), sizeof(Identification));

		return 0;
	}
	// 采用控制权思想，保证Data数据主动权在FTP池中
	int MFTP::package()
	{
		return 0;

	}
	int MFTP::disconnect()
	{
		return 0;
	}

	//数据包创建
	Identification* MFTP::createIdentification()
	{
		return nullptr;
	}
	Data* MFTP::createData()
	{
		return nullptr;

	}
	Termination* MFTP::createTermination()
	{
		return nullptr;

	}


};