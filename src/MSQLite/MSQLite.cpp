#include"MSQLiite.h"
#include"MRBTree.h"
#include<string>
#include<compare>
#include<boost/filesystem.hpp>
#include"../MBase/MError.h"
#include<stdint.h>
#include<codecvt>
#include<stdarg.h>
#include<vector>
namespace MUZI
{
	struct MSQLite::__SQL_TABLE__
	{
		size_t attributes_num;// 属性个数
		unsigned char attribute_type[__MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__];// 变量类型数组
		unsigned char attribute_size[__MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__];// 变量大小数组, 如果是字符串则是最大字符串char长度

	};
	struct MSQLite::__SQL_MESSAGE__
	{
	public:
		MSQLite::sql_id_t sql_id;// id
		MSQLite::sql_type_t sql_type;// 类型
		sqlite3_stmt* pstmt;// 预编译语句
		size_t args_num;// 表示语句中？的个数
		unsigned char attribute_type[__MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__];// 变量类型数组, 仅支持int64 utf8 double等
		__SQL_TABLE__* table;
	};
	struct MSQLite::__MSQLite_Data__
	{
		struct MSQLite::__SQL_MESSAGE__* sqls;
		sqlite3* sq3;

	};

	// MSelectResult
	MSQLite::MSelectResult::MSelectResult(char* stream, int* index_list, size_t attribute_num, size_t size)
		:bin_data_stream(stream), 
		index_list(index_list), 
		attribute_num(attribute_num), 
		size (size), 
		objectnum(new int(1))
	{}

	MSQLite::MSelectResult::MSelectResult()
		:bin_data_stream(nullptr),
		index_list(nullptr),
		attribute_num(0),
		size(0),
		objectnum(nullptr)
	{}

	MSQLite::MSelectResult::MSelectResult(MSelectResult& that)
		:bin_data_stream(that.bin_data_stream),
		index_list(that.index_list),
		attribute_num(that.attribute_num),
		size(that.size),
		objectnum(that.objectnum)
	{
		*that.objectnum += 1;
	}

	MSQLite::MSelectResult::MSelectResult(MSelectResult&& that)
		:bin_data_stream(that.bin_data_stream),
		index_list(that.index_list),
		attribute_num(that.attribute_num),
		size(that.size),
		objectnum(that.objectnum)
	{
		that.bin_data_stream = nullptr;
		that.index_list = nullptr;
		that.attribute_num = 0;
		that.size = 0;
		that.objectnum = nullptr;
	}

	MSQLite::MSelectResult::~MSelectResult()
	{
		if (--*this->objectnum == 0)
		{
			if (this->bin_data_stream != nullptr)
			{
				delete this->bin_data_stream;
				this->bin_data_stream = nullptr;
			}
			if (this->index_list != nullptr)
			{
				delete this->index_list;
				this->index_list = nullptr;
			}
			delete this->objectnum;
		}
	}

	int32_t MSQLite::MSelectResult::getINT(DataStream data, int& err)
	{
		if (data == nullptr)
		{
			return MERROR::ARG_IS_NULL;
		}
		char* p_data = static_cast<char*>(data);
		// 采用在每个数据的头部增加int检测位，来自诉数据大小
		if (*(int*)p_data != sizeof(int32_t))
		{
			err = MERROR::TYPE_ERR;
			return int32_t();
		}
		p_data += sizeof(int);
		return *reinterpret_cast<int32_t*>(p_data);
	}

	int64_t MSQLite::MSelectResult::getINT64(DataStream data, int& err)
	{
		if (data == nullptr)
		{
			return MERROR::ARG_IS_NULL;
		}
		char* p_data = static_cast<char*>(data);
		if (*(int*)p_data != sizeof(int64_t))
		{
			err = MERROR::TYPE_ERR;
			return int64_t();
		}
		p_data += sizeof(int);

		return *reinterpret_cast<int64_t*>(p_data);

	}

	double MSQLite::MSelectResult::getDOUBLE(DataStream data, int& err)
	{
		if (data == nullptr)
		{
			return MERROR::ARG_IS_NULL;
		}
		char* p_data = static_cast<char*>(data);
		if (*(int*)p_data != sizeof(double))
		{
			err = MERROR::TYPE_ERR;
			return double();
		}
		p_data += sizeof(int);

		return *reinterpret_cast<double*>(p_data);
	}

	int MSQLite::MSelectResult::getTEXT(DataStream data, char* ret_buff, int& err)
	{
		if (data == nullptr || ret_buff == nullptr)
		{
			return MERROR::ARG_IS_NULL;
		}
		char* p_data = static_cast<char*>(data);
		int size = *(int*)p_data;
		p_data += sizeof(int);
		if (memcpy_s(ret_buff, size, p_data, size) != 0)
		{
			return MERROR::OUT_OF_BUFF;
		}
		return 0;
	}

	int MSQLite::MSelectResult::getTEXT16(DataStream data, char* ret_buff, int& err)
	{
		if (data == nullptr || ret_buff == nullptr)
		{
			return MERROR::ARG_IS_UNEXPECTED;
		}
		char* p_data = static_cast<char*>(data);
		int size = *(int*)p_data;
		p_data += sizeof(int);
		if (memcpy_s(ret_buff, size, p_data, size) != 0)
		{
			return MERROR::OUT_OF_BUFF;
		}
		return 0;
	}

	void* MSQLite::MSelectResult::getEleByColnum(int index, size_t& attribute_size, int& err)
	{
		if (index > this->attribute_num)
		{
			err = MERROR::OUT_OF_RANGE;
			return nullptr;
		}
		return &this->bin_data_stream[this->index_list[index]];
	}

	void MSQLite::MSelectResult::setDataStream(char* data_stream, size_t size)
	{
		if (this->objectnum == nullptr)
		{
			this->objectnum = new int(1);
		}
		if (this->bin_data_stream == nullptr)
		{
			this->bin_data_stream = data_stream;
			this->size = size;
		}
	}

	void MSQLite::MSelectResult::setIndexList(int* index_list, size_t attributeNum)
	{
		if (this->objectnum == nullptr)
		{
			this->objectnum = new int(1);
		}
		if (this->index_list != nullptr)
		{
			this->index_list = index_list;
			this->attribute_num = attributeNum;
		}
	}



	// MSQLite
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
		return 0;
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
		va_start(args, sql_id);// va_start需要是最后一个有名参数


		switch (sql->sql_type)
		{
		case SQL_SELECT:
		{
			int ret_data_reserve_szie = 100;
			std::vector<MSQLite::MSelectResult>* ret_datas = va_arg(args, std::vector<MSQLite::MSelectResult>*);
			ret_datas->reserve(ret_data_reserve_szie);
			// 填充预编译语句
			for (int i = 0; i < sql->args_num; ++i)
			{
				switch (sql->attribute_type[i])
				{
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

			}
			int rc = 0;

			size_t data_stream_size = 0;
			// 获取表单类型大小，以设置数据流大小
			for (int i = 0; i < sql->table->attributes_num; ++i)
			{
				data_stream_size += sql->table->attribute_size[i];
			}

			data_stream_size = data_stream_size + sql->table->attributes_num * sizeof(int);
			char* data_stream = nullptr;
			char* p_data_stream_index = nullptr;
			int* index_list = nullptr;
			int index_list_size = 0;
			int attribute_index = 0;
			int loop_index = 0;
			int sqlite_colnum = 0;

			// 向数据流填充数据
			// 该判断语句代表由匹配的行
			while ((rc = sqlite3_step(sql->pstmt)) == SQLITE_ROW) {
				data_stream = new char[data_stream_size];
				p_data_stream_index = data_stream;
				int* index_list = new int[index_list_size];

				for (int i = 0; i < sql->table->attributes_num; ++i)
				{
					switch (sqlite3_column_type(sql->pstmt, sqlite_colnum))
					{
					case SQLITE_INTEGER:
					{
						*(int*)(p_data_stream_index) = sizeof(int32_t);
						p_data_stream_index += sizeof(int);
						*(int*)(p_data_stream_index) = sqlite3_column_int(sql->pstmt, sqlite_colnum++);
						p_data_stream_index += sizeof(int32_t);
						break;
					}
					case SQLITE_TEXT:
					{
						const unsigned char* tmp_text = sqlite3_column_text(sql->pstmt, sqlite_colnum);
						int tmp_text_len = sqlite3_column_bytes(sql->pstmt, sqlite_colnum++);
						*(int*)(p_data_stream_index) = tmp_text_len + 2; // \0终止符也要添加进去
						p_data_stream_index += sizeof(int);
						memcpy(p_data_stream_index, tmp_text, tmp_text_len);
						p_data_stream_index += tmp_text_len;
						*p_data_stream_index++ = '\0';// utf-8 由两个\0结尾
						*p_data_stream_index++ = '\0';
						break;
					}
					case SQLITE_FLOAT:
					{
						*(int*)(p_data_stream_index) = sizeof(double);
						p_data_stream_index += sizeof(int);
						*(int*)(p_data_stream_index) = sqlite3_column_int64(sql->pstmt, sqlite_colnum++);
						p_data_stream_index += sizeof(double);
						break;
					}
					case SQLITE_NULL:
					{
						*(int*)(p_data_stream_index) = -1;
						p_data_stream_index += sizeof(int);
						break;
					}
					default:
						va_end(args);
						return MERROR::ARG_IS_UNEXPECTED;
						break;
					}

				}
				++loop_index;
				ret_datas->emplace(ret_datas->end(), MSelectResult(p_data_stream_index, index_list, data_stream_size, index_list_size));
				if (ret_datas->size() >= ret_data_reserve_szie)
				{
					ret_data_reserve_szie *= 2;
					ret_datas->reserve(ret_data_reserve_szie);
				}
				sqlite3_reset(sql->pstmt);
				sqlite_colnum = 0;
			}


			break;
		}
		case SQL_DELETE:
		{
			for (int i = 0; i < sql->args_num; ++i)
			{
				switch (sql->attribute_type[i])
				{
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

			}
			sqlite3_step(sql->pstmt);
			sqlite3_reset(sql->pstmt);
			break;
		}
		case SQL_INSERT:
		{
			for (int i = 0; i < sql->args_num; ++i)
			{
				switch (sql->attribute_type[i])
				{
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

			}
			sqlite3_step(sql->pstmt);
			sqlite3_reset(sql->pstmt);
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

	void MSQLite::MSelectResultFinalize(MSQLite::MSelectResult**)
	{

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

