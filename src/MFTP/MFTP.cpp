#include"MFTP.h"
#include"../MBase/MError.h"
namespace MUZI
{
	// ����
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


	// ���캯��
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

	//���Ӻ���
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
	// ���ÿ���Ȩ˼�룬��֤Data��������Ȩ��FTP����
	int MFTP::package()
	{
		return 0;

	}
	int MFTP::disconnect()
	{
		return 0;
	}

	//���ݰ�����
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