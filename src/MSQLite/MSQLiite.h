#ifndef __MUZI_MSQLITE_H__
#define __MUZI_MSQLITE_H__
#include<sqlite3.h>

#define __MUZI_MSQLITE_SQL_CODE_SIZE__ 1024
#define __MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__ 8
namespace MUZI
{
	// 该类只能绑定一个db数据库
	class MSQLite
	{
	public:
		enum SQLType
		{
			SQL_SELECT = 100,
			SQL_CREATE,
			SQL_DELETE,
			SQL_INSERT
		};
		enum __SQLAttributeType__;
	private:
		struct __SQL_MESSAGE__;
		struct __MSQLite_Data__;
	public:
		using sql_id_t = size_t;
		using sql_type_t = size_t;
	public:
		MSQLite();
		MSQLite(const MSQLite&) = delete;
		MSQLite(MSQLite&&);
		~MSQLite();
	public:
		int setMode();
	public:
		int createDB(const char* database_name);
		int createTable(const char* sql);
	public:
		int bindDB();
	public:
		int driverSQL(const char* sql);
	public:
		sql_id_t registerSQL(const char* sql, int type);
		sql_type_t getSQLType(sql_id_t sql_id);
		int driverSQL(sql_id_t sql_id, void* data, size_t data_size, ...);
	public:
		inline bool isSELECTT(sql_type_t);
		inline bool isCREATE(sql_type_t);
		inline bool isDELETE(sql_type_t);
		inline bool isINSERT(sql_type_t);
		inline bool isErrorType(sql_type_t);
	private:
		size_t __analizeSQL__(); 
		struct __SQL_MESSAGE__* __checkSQLID__(sql_id_t sql_id, int& err_no);

	private:
		struct __MSQLite_Data__* m_data;
	};
}



#endif // !__MUZI_MSQLITE_H__
