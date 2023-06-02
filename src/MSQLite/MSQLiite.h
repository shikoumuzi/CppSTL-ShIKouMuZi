#ifndef __MUZI_MSQLITE_H__
#define __MUZI_MSQLITE_H__
#include<string>
#include<sqlite3.h>

#define __MUZI_MSQLITE_SQL_CODE_SIZE__ 1024
#define __MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__ 8
namespace MUZI
{
	// 该类只能绑定一个db数据库
	class MSQLite
	{
	public:
		class MSelectResult
		{
			MSelectResult(char*, size_t size);
			MSelectResult(const MSelectResult&) = delete;
			MSelectResult(MSelectResult&&) = delete;
			~MSelectResult();
		public:
			int32_t getINT(int& err);
			int64_t getINT64(int& err);
			double getDOUBLE(int& err);
			char* getTEXT(int& err);
			char* getTEXT16(int& err);
		private:
			inline int __EndOfDistance__(char* now);
			inline bool __checkOutOfRange__(char* now);
		public:
			char* bin_data_stream;
			size_t index;
			size_t size;
		};

	public:
		using String = std::u8string;
		using sql_id_t = size_t;
		using sql_type_t = size_t;
		using ReceiveSelectResultFun = void(*)(MSelectResult*, sql_id_t, ...);
	public:
		enum SQLType
		{
			SQL_SELECT = 100,
			SQL_CREATE,
			SQL_DELETE,
			SQL_INSERT
		};
		enum __SQLAttributeType__
		{
			INT = 1,
			DOUBLE = 2,
			INT64 = 4,
			_NULL = 8,
			TEXT = 16,
			TEXT16 = 32
		};
	private:
		struct __SQL_MESSAGE__;
		struct __MSQLite_Data__;
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

		int driverSQL(sql_id_t sql_id, ...);
		/*
		* if mode is SELECT, you should set a callback function witch is get a row data from DataBase in the first arg after arg: sql_id, and then you should set your attribute by pointer 
		* if mode is DELETE/INSERT you should set your attribute by pointer after arg: sql_id
		*/
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
