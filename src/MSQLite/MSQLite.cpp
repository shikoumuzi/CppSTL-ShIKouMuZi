#include"MSQLiite.h"
#include"MRBTree.h"
#include<string>
#include<compare>
#include<boost/filesystem.hpp>
#include"../MBase/MError.h"
namespace MUZI
{
	struct MSQLite::__SQL_MESSAGE__
	{
	public:
		MSQLite::sql_id_t sql_id;
		MSQLite::sql_type_t sql_type;
		sqlite3_stmt* data;
		size_t attribute_num;
	};
	struct MSQLite::__MSQLite_Data__
	{
		struct MSQLite::__SQL_MESSAGE__* sqls;
		sqlite3* sq3;
	};

	MSQLite::MSQLite():m_data(new struct MSQLite::__MSQLite_Data__)
	{
		boost::filesystem::current_path(boost::filesystem::initial_path<boost::filesystem::path>());
		this->m_data->sqls = new struct __SQL_MESSAGE__[__MUZI_MSQLITE_SQL_CODE_SIZE__];
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

	int MSQLite::createDB()
	{




		return 0;
	}

	int MSQLite::createTable()
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

	}
	int MSQLite::driverSQL(sql_id_t sql_id, void* data, size_t data_size, ...)
	{
		if (sql_id >= __MUZI_MSQLITE_SQL_CODE_SIZE__)
		{
			return MERROR::SQL_ID_ERR;
		}
		__SQL_MESSAGE__& sql = this->m_data->sqls[sql_id];
		if (sql.sql_id != sql_id)
		{
			return MERROR::SQL_NO_FOUND;
		}
		
	}

	size_t MSQLite::__analizeSQL__()
	{
		return size_t();
	}

}

