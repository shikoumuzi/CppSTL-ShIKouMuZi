#include"MSQLiite.h"
#include"MRBTree.h"
#include<string>
#include<compare>
#include<boost/filesystem.hpp>
#include"../MBase/MError.h"
#include<stdint.h>
namespace MUZI
{
	struct MSQLite::__SQL_MESSAGE__
	{
	public:
		MSQLite::sql_id_t sql_id;
		MSQLite::sql_type_t sql_type;
		sqlite3_stmt* data;
		size_t attribute_num;
		char attribute_type[__MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__];
	};
	struct MSQLite::__MSQLite_Data__
	{
		struct MSQLite::__SQL_MESSAGE__* sqls;
		sqlite3* sq3;

	};
	enum MSQLite::__SQLAttributeType__
	{
		INT = 1,
		DOUBLE,
		INT64,
		_NULL,
		TEXT,
		TEXT16
	};

	MSQLite::MSQLite():m_data(new struct MSQLite::__MSQLite_Data__)
	{
		boost::filesystem::current_path(boost::filesystem::initial_path<boost::filesystem::path>());
		this->m_data->sqls = new struct __SQL_MESSAGE__[__MUZI_MSQLITE_SQL_CODE_SIZE__];
		__SQL_MESSAGE__ sql_message = {-1, 0, nullptr, 0};
		for (int i = 0; i < __MUZI_MSQLITE_SQL_CODE_SIZE__; ++i)
		{
			memcpy(&this->m_data->sqls[i], &sql_message, sizeof(__SQL_MESSAGE__));
		}
	}
	MSQLite::MSQLite(MSQLite&& that):m_data(that.m_data)
	{
		that.m_data = nullptr;
	}
	MSQLite::~MSQLite()
	{
		if (this->m_data != nullptr)
		{
			if (this->m_data->sqls != nullptr)
			{
				delete this->m_data->sqls;
			}
			delete this->m_data;
		}
	}

	int MSQLite::setMode()
	{

	}

	int MSQLite::createDB(const char* database_name)
	{




		return 0;
	}

	int MSQLite::createTable(const char* sql)
	{

	}

	int MSQLite::bindDB()
	{
		return 0;
	}

	int MSQLite::driverSQL(const char* sql)
	{

	}

	MSQLite::sql_id_t MSQLite::registerSQL(const char* sql, int type)
	{

	}
	MSQLite::sql_type_t MSQLite::getSQLType(sql_id_t sql_id)
	{
		int err_no = 0;
		__SQL_MESSAGE__* sql = this->__checkSQLID__(sql_id, err_no);
		if (sql == nullptr)
		{
			return err_no;
		}
		return sql->sql_type;
	}
	int MSQLite::driverSQL(sql_id_t sql_id, void* data, size_t data_size, ...)
	{
		int err_no = 0;
		__SQL_MESSAGE__* sql = this->__checkSQLID__(sql_id, err_no);
		if (sql == nullptr)
		{
			return err_no;
		}
		va_list args; // 定义一个va_list类型的变量
		va_start(args, sql->attribute_num);
		
		for (int i = 0; i < sql->attribute_num; ++i)
		{
			switch (sql->attribute_type[i])
			{
			case __SQLAttributeType__::INT:
				break;
			case __SQLAttributeType__::INT64:
				break;
				case __SQLAttributeType__::TEXT
			default:
				break;
			}
		}


		va_end(args);
	}

	size_t MSQLite::__analizeSQL__()
	{
		return size_t();
	}

	struct MSQLite::__SQL_MESSAGE__* MSQLite::__checkSQLID__(sql_id_t sql_id, int& err_no)
	{
		if (sql_id >= __MUZI_MSQLITE_SQL_CODE_SIZE__ || sql_id < 0)
		{
			err_no = MERROR::SQL_ID_ERR;
			return nullptr;
		}
		__SQL_MESSAGE__* sql = &this->m_data->sqls[sql_id];
		if (sql->sql_id != sql_id)
		{
			err_no = MERROR::SQL_NO_FOUND;
			return nullptr;
		}
		return sql;
	}
	bool MSQLite::isSELECTT(sql_type_t sql_type)
	{
		return sql_type == SQLType::SQL_SELECT;
	}
	bool MSQLite::isCREATE(sql_type_t sql_type)
	{
		return sql_type == SQLType::SQL_CREATE;
	}

	bool MSQLite::isDELETE(sql_type_t sql_type)
	{
		return sql_type == SQLType::SQL_DELETE;
	}

	bool MSQLite::isINSERT(sql_type_t sql_type)
	{
		return sql_type == SQLType::SQL_INSERT;
	}

	bool MSQLite::isErrorType(sql_type_t sql_type)
	{
		return sql_type > SQLType::SQL_INSERT || sql_type < SQLType::SQL_SELECT;
	}

}

