#ifndef __MUZI_MSQLITE_H__
#define __MUZI_MSQLITE_H__
#include<string>
#include<sqlite3.h>

#define __MUZI_MSQLITE_SQL_CODE_SIZE__ 1024
#define __MUZI_MSQLITE_MAX_ATTRIBUTE_SIZE__ 8
#define __MUZI_MSQLITE_MAX_TEXT_BYTES_SIZE__ 2048
namespace MUZI
{
	// 该类只能绑定一个db数据库
	class MSQLite
	{
	public:
		class MSelectResult
		{
		public:
			using DataStream = void*;
		public:
			MSelectResult();
			MSelectResult(char*, int*, size_t, size_t size);
			MSelectResult(MSelectResult&);
			MSelectResult(const MSelectResult&) = delete;
			MSelectResult(MSelectResult&&);
		public:
			~MSelectResult();
		public:
			int32_t getINT(DataStream data, int& err);
			int64_t getINT64(DataStream data, int& err);
			double getDOUBLE(DataStream data, int& err);
			int getTEXT(DataStream data, char* ret_buff, int& err);
			int getTEXT16(DataStream data, char* ret_buff, int& err);
			
			/// @brief this function should input an index to get an void* data from data stream
			/// @param index should start at 1
			/// @return an void* pointer which is point to data address
			DataStream getEleByColnum(int index, size_t& data_size, int& err);
		private:
			void setDataStream(char* data_stream, size_t size);
			void setIndexList(int* index_list, size_t attributeNum);
		public:
			char* bin_data_stream;
			int* index_list;
			size_t size;
			size_t attribute_num;
			int* objectnum;
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
			DOUBLE = 2,
			INT64 = 4,
			_NULL = 8,
			TEXT = 16,
		};
	private:
		struct __SQL_TABLE__;
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
		int driverSQL(const char* sql, int(*callback)(void*, int, char**, char**), void* flag);
	public:
		
		sql_id_t registerSQL(const char* sql, int type);
		
		
		sql_type_t getSQLType(sql_id_t sql_id);
		/// @brief this function will driver the sql code by sql_id from registerSQL()
		/// @param sql_id 
		/// @param  to insert or select value
		/// @return  if mode is SELECT, you should set a callback function witch is get a row data from DataBase in the first arg after arg: sql_id, and then you should set your attribute by pointer 
		/// if mode is DELETE/INSERT you should set your attribute by pointer after arg: sql_id
		int driverSQL(sql_id_t sql_id, ...);

	public:
		void MSelectResultFinalize(MSQLite::MSelectResult**);

	public:
		/// @brief this function to get a sql type(SELECT)
		/// @param should get by getSQLType
		/// @return if is SELECT will return true, other will return false
		inline bool isSELECTT(sql_type_t);
		inline bool isCREATE(sql_type_t);
		inline bool isDELETE(sql_type_t);
		inline bool isINSERT(sql_type_t);

		inline bool isErrorType(sql_type_t);
	private:

		size_t __analizeSQL__(); 
		/// @brief 
		/// @param sql_id 
		/// @param err_no 
		/// @return 
		struct __SQL_MESSAGE__* __checkSQLID__(sql_id_t sql_id, int& err_no);

	private:
		struct __MSQLite_Data__* m_data;
	};
}



#endif // !__MUZI_MSQLITE_H__
