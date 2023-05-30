#ifndef __MUZI__MFILEDATABASE_H__
#define __MUZI__MFILEDATABASE_H__
#include<boost/filesystem.hpp>
#include<boost/filesystem/fstream.hpp>
#include<boost/system/error_code.hpp>
#include<string.h>
#include"MBase/MError.h"
#include"MFileOutput.h"
#include<MPoolAllocator.h>
#include<boost/asio/thread_pool.hpp>
#include<thread>
#include<mutex>
#include<atomic>
#include<condition_variable>
#include"MBitmapAllocator.h"
#define __MUZI_MFILEDATABASE_SQL_PAGE_TABLE_SIZE__ 256
#define __MUZI_MFILEDATABASE_SQL_BUF_SIZE__ 256
namespace MUZI
{

	class MFileDataBase
	{
	public:
		using String = std::string;
		using Fstream = boost::filesystem::fstream;
		using FRstream = MFileOutput;
		using Path = boost::filesystem::path;
		using GetFileStaus = boost::filesystem::file_status(*)(Path const &);
		using BError = boost::system::error_code;
		static GetFileStaus getFileStatus;
		using Thread = std::thread;
		using Mutex = std::unique_lock<std::mutex>;
		using Condition = std::condition_variable;
	private:
		struct __MFileDataBase_Data__;
	public:
		MFileDataBase(const char* sqlite_dir_path = "./sqlite");
		MFileDataBase(const String& sqlite_dir_path);
		MFileDataBase(const MFileDataBase&) = delete;
		MFileDataBase(MFileDataBase&&)noexcept;
		~MFileDataBase();
	public:
		// bind a root dir to construct an file database
		int bind(const char* root);
		int bind(const String& root);
		
		// create file database base on the binding message
		int constructDataBase(const char* sqlite_name = "./sqlite/sqlite.sqlite");
		int constructDataBase(const String& sqlite_name);

		// get file from base message
		FRstream& readFile(const char* file_name);
		FRstream& readFile(const String& file_name);
		FRstream& readFileWithoutFormat(const char* file_name);
		FRstream& readFileWithoutFormat(const String& file_name);
	
		// write file
		int writeFile(const char* file_name, const char* format, Fstream& fstream);
		int writeFile(const String& file_name, const String& format, Fstream& fstream);

		// earse dir
		int earseFile(const char* dir);
		int earseFile(const String& dir);

		// get Dir like a file from base message
		Fstream& readDir(const char* dir_name);
		Fstream& readDir(const String& dir_name);

		// write Dir like a file from base message
		int writeDir(const char* dir_name, const char* format, int file_count, int file_size);
		int writeDir(const String& dir_name, const String& format, Fstream& fstream);

		// earse dir
		int earseDir(const char* dir);
		int earseDir(const String& dir);
		
	private:
		enum __SqlType__
		{
			SQL_CREATE = 0,
			SQL_DELETE,
			SQL_INSERT,
			SQL_SELECT
		};
		static int sql_callback(void* para, int columenCount, char** columnValue, char** columnName);
		int createSql(size_t type);
		int deleteSql(char* data);

	private:
		void __delete__();
	private:

		MPoolAllocator alloc;
		struct __MFileDataBase_Data__* m_data;
	};
};


#endif // !__MUZI__MFILEDATABASE_H__


