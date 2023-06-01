#include"MSQLiite.h"
#include"MRBTree.h"
#include<string>
#include<compare>
#include<boost/filesystem.hpp>
#include"../MBase/MError.h"
#include<stdint.h>
#include<codecvt>
namespace MUZI
{
	struct MSQLite::__SQL_MESSAGE__
	{
	public:
		MSQLite::sql_id_t sql_id;
		MSQLite::sql_type_t sql_type;
		sqlite3_stmt* pstmt;
		size_t attribute_num;
		unsigned char attribute_type[__MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__];
	};
	struct MSQLite::__MSQLite_Data__
	{
		struct MSQLite::__SQL_MESSAGE__* sqls;
		sqlite3* sq3;

	};
	enum MSQLite::__SQLAttributeType__
	{
		INT = 1,
		DOUBLE = 2,
		INT64 = 4,
		_NULL = 8,
		TEXT = 16,
		TEXT16 = 32
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
				for (int i = 0; i < __MUZI_MSQLITE_SQL_CODE_SIZE__; ++i)
				{
					if (this->m_data->sqls->pstmt != nullptr)
					{
						sqlite3_finalize(this->m_data->sqls->pstmt);
						this->m_data->sqls->pstmt = nullptr;
					}
				}

				delete this->m_data->sqls;
				this->m_data->sqls = nullptr;
			}
			if (this->m_data->sq3)
			{
				sqlite3_close_v2(this->m_data->sq3);
				this->m_data->sq3 = nullptr;
			}
			delete this->m_data;
			this->m_data = nullptr;
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


		return 0;
	}

	int MSQLite::bindDB()
	{
		return 0;
	}

	int MSQLite::driverSQL(const char* sql)
	{


		return 0;
	}

	MSQLite::sql_id_t MSQLite::registerSQL(const char* sql, int type)
	{


		return 0;
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
	int MSQLite::driverSQL(sql_id_t sql_id, ...)
	{
		int err_no = 0;
		sqlite3** sq3 = &this->m_data->sq3;
		__SQL_MESSAGE__* sql = this->__checkSQLID__(sql_id, err_no);
		if (sql == nullptr)
		{
			return err_no;
		}

		int attribute_type = 0;

		va_list args; // 定义一个va_list类型的变量
		va_start(args, sql->attribute_num);
		
		switch (sql->sql_type)
		{
		case SQL_SELECT:
		{
			for (int i = 0; i < sql->attribute_num; ++i)
			{
				switch (sql->attribute_type[i])
				{
				case __SQLAttributeType__::INT:
				{
					int* arg = va_arg(args, int*);
					sqlite3_bind_int(sql->pstmt, i, *arg);
					if (arg == nullptr)
					{
						goto __SQL_SELECT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::INT64:
				{
					int64_t* arg = va_arg(args, int64_t*);
					sqlite3_bind_int64(sql->pstmt, i + 1, *arg);
					if (arg == nullptr)
					{
						goto __SQL_SELECT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::TEXT:
				{
					const char* arg = va_arg(args, const char*);
					sqlite3_bind_text(sql->pstmt, i + 1, arg, -1, SQLITE_STATIC);
					if (arg == nullptr)
					{
						goto __SQL_SELECT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::TEXT16:
				{
					const char* arg = va_arg(args, const char*);
					sqlite3_bind_text(sql->pstmt, i + 1, arg, -1, SQLITE_STATIC);
					if (arg == nullptr)
					{
						goto __SQL_SELECT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::DOUBLE:
				{
					double* arg = va_arg(args, double*);
					sqlite3_bind_double(sql->pstmt, i + 1, *arg);
					if (arg == nullptr)
					{
						goto __SQL_SELECT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::_NULL:
				{
				__SQL_SELECT_ARG_IS_NULL__:
					sqlite3_bind_null(sql->pstmt, i + 1);
					break;
				}
				default:
					va_end(args);
					return MERROR::ARG_IS_UNEXPECTED;
					break;
				}
				sqlite3_step(sql->pstmt);
				sqlite3_reset(sql->pstmt);
			}
			break;
		}
		case SQL_DELETE:
		{
			for (int i = 0; i < sql->attribute_num; ++i)
			{
				switch (sql->attribute_type[i])
				{
				case __SQLAttributeType__::INT:
				{
					int* arg = va_arg(args, int*);
					sqlite3_bind_int(sql->pstmt, i, *arg);
					if (arg == nullptr)
					{
						goto __SQL_DELETE_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::INT64:
				{
					int64_t* arg = va_arg(args, int64_t*);
					sqlite3_bind_int64(sql->pstmt, i + 1, *arg);
					if (arg == nullptr)
					{
						goto __SQL_DELETE_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::TEXT:
				{
					const char* arg = va_arg(args, const char*);
					sqlite3_bind_text(sql->pstmt, i + 1, arg, -1, SQLITE_STATIC);
					if (arg == nullptr)
					{
						goto __SQL_DELETE_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::TEXT16:
				{
					const char* arg = va_arg(args, const char*);
					sqlite3_bind_text(sql->pstmt, i + 1, arg, -1, SQLITE_STATIC);
					if (arg == nullptr)
					{
						goto __SQL_DELETE_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::DOUBLE:
				{
					double* arg = va_arg(args, double*);
					sqlite3_bind_double(sql->pstmt, i + 1, *arg);
					if (arg == nullptr)
					{
						goto __SQL_DELETE_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::_NULL:
				{
				__SQL_DELETE_ARG_IS_NULL__:
					sqlite3_bind_null(sql->pstmt, i + 1);
					break;
				}
				default:
					va_end(args);
					return MERROR::ARG_IS_UNEXPECTED;
					break;
				}
				sqlite3_step(sql->pstmt);
				sqlite3_reset(sql->pstmt);
			}
			break;
		}
		case SQL_INSERT:
		{
			for (int i = 0; i < sql->attribute_num; ++i)
			{
				switch (sql->attribute_type[i])
				{
				case __SQLAttributeType__::INT:
				{
					int* arg = va_arg(args, int*);
					sqlite3_bind_int(sql->pstmt, i, *arg);
					if (arg == nullptr)
					{
						goto __SQL_INSERT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::INT64:
				{
					int64_t* arg = va_arg(args, int64_t*);
					sqlite3_bind_int64(sql->pstmt, i + 1, *arg);
					if (arg == nullptr)
					{
						goto __SQL_INSERT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::TEXT:
				{
					const char* arg = va_arg(args, const char*);
					sqlite3_bind_text(sql->pstmt, i + 1, arg, -1, SQLITE_STATIC);
					if (arg == nullptr)
					{
						goto __SQL_INSERT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::TEXT16:
				{
					const char* arg = va_arg(args, const char*);
					sqlite3_bind_text(sql->pstmt, i + 1, arg, -1, SQLITE_STATIC);
					if (arg == nullptr)
					{
						goto __SQL_INSERT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::DOUBLE:
				{
					double* arg = va_arg(args, double*);
					sqlite3_bind_double(sql->pstmt, i + 1, *arg);
					if (arg == nullptr)
					{
						goto __SQL_INSERT_ARG_IS_NULL__;
					}
					break;
				}
				case __SQLAttributeType__::_NULL:
				{
				__SQL_INSERT_ARG_IS_NULL__:
					sqlite3_bind_null(sql->pstmt, i + 1);
					break;
				}
				default:
					va_end(args);
					return MERROR::ARG_IS_UNEXPECTED;
					break;
				}
				sqlite3_step(sql->pstmt);
				sqlite3_reset(sql->pstmt);
			}
			break;
		}
		default:
			va_end(args);
			return MERROR::ARG_IS_UNEXPECTED;
			break;
		}
		va_end(args);
		return 0;
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

