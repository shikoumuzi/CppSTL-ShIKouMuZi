#ifndef __MUZI_MSQLITE_H__
#define __MUZI_MSQLITE_H__
#include<sqlite3.h>
namespace MUZI
{
	// 该类只能绑定一个db数据库
	class MSQLite
	{
	private:
		struct __SQL_MESSAGE__;
		struct __MSQLite_Data__;
	public:
		using sql_id_t = size_t;
		using sql_type_t = size_t;
	public:
		MSQLite(const char* sqlite_dir_path = "./sqlite");
		MSQLite(const MSQLite&) = delete;
		MSQLite(MSQLite&&);
		~MSQLite();
	public:
		int setMode();
	public:
		int createTable();
	public:
		int driverSQL(const char* sql);
	public:
		sql_id_t registerSQL(const char* sql, int type);
		sql_type_t getSQLType(sql_id_t sql_id);
		int driverSQL(sql_id_t sql_id, ...);
	private:
		struct __MSQLite_Data__* m_data;
	};
}



#endif // !__MUZI_MSQLITE_H__
