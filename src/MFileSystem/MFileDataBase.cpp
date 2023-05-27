#include"MFileDataBase.h"
#include<cstring>
#include<sqlite3.h>

namespace MUZI
{

	struct MFileDataBase::__MFileDataBase_Data__
	{
		char* root;
		char* sqlite_dir;
		sqlite3* sq3;
		struct Sql_Thread
		{
			Thread* sql_exec_th;
			Mutex* sql_exec_lock;
			Condition* sql_exec_cond;
			bool THREAD_WORK;
		}sql_thread;

	};
	
	struct __MFileDataBase_Sql_Table__
	{
		char* filename;
		char* filepath;
	};

	struct __MFileDataBase_Sql_Page__
	{
		int pageid;
		int table_szie;
		struct __MFileDataBase_Sql_Table__ tables[__MUZI_MFILEDATABASE_SQL_PAGE_TABLE_SIZE__];
	};


	struct __MFileDataBase_Sql_Task__
	{
		int type;
		void* data;
	};

	int MFileDataBase::sql_callback(void* para, int columenCount, char** columnValue, char** columnName)
	{

	}

	MFileDataBase::GetFileStaus MFileDataBase::getFileStatus = boost::filesystem::status;

	MFileDataBase::MFileDataBase(const char* sqlite_dir_path) :m_data(new struct __MFileDataBase_Data__({nullptr, nullptr, nullptr, nullptr}))
	{
		Path path(sqlite_dir_path);
		if (!boost::filesystem::exists(path))
		{
			boost::filesystem::create_directory(path);
		}
	}
	MFileDataBase::MFileDataBase(const String& sqlite_dir_path): MFileDataBase(sqlite_dir_path.c_str()) {}
	MFileDataBase::MFileDataBase(MFileDataBase&& that)noexcept : m_data(that.m_data)
	{
		that.m_data = nullptr;
	}
	MFileDataBase::~MFileDataBase()
	{
		if (this->m_data->sq3 != nullptr)
		{
			sqlite3_close(this->m_data->sq3);
		}
		if (this->m_data->root != nullptr)
		{
			this->alloc.deallocate(this->m_data->root, strlen(this->m_data->root) + 1);
		}
		if (this->m_data->sql_thread.sql_exec_th != nullptr)
		{
			this->m_data->sql_thread.THREAD_WORK = false;
			delete this->m_data->sql_thread.sql_exec_th;
		}
		if (this->m_data->sql_thread.sql_exec_lock != nullptr)
		{

		}
		if (this->m_data->sql_thread.sql_exec_cond != nullptr)
		{
			this->m_data->sql_thread.sql_exec_cond->notify_all();
			delete this->m_data->sql_thread.sql_exec_cond;
		}
	}

	// bind a root dir to construct an file database
	int MFileDataBase::bind(const char* root)
	{
		BError ec;
		Path root_path(root);
		if (boost::filesystem::exists(root_path))
		{
			return MERROR::DIRNOEXITS;
		}
		if (boost::filesystem::is_directory(root_path, ec))
		{
			size_t path_name_len = strlen(root) + 1;
			this->m_data->root = static_cast<char*>(this->alloc.allocate(path_name_len));
			memcpy(static_cast<void*>(this->m_data->root), root, path_name_len);
			return 0;
		}
		return MERROR::NOADIR;
	}
	int MFileDataBase::bind(const String& root)
	{
		return this->bind(root.c_str());
	}

	// create file database base on the binding message
	int MFileDataBase::constructDataBase(const char* sqlite_path) 
	{
		char err_msg[256] = { '\0' };
		char sql_buff[256] = { '\0' };
		if (sqlite3_open(sqlite_path, &this->m_data->sq3) )
		{
			return MERROR::SQLITEOPENERR;
		}
		if (sqlite3_exec(this->m_data->sq3, sql_buff, this->sql_callback, nullptr, (char**) & err_msg))
		{

		}


		String sql_str("");
		__MFileDataBase_Sql_Page__ table({0, 0});
		__MFileDataBase_Sql_Task__ task({ __SqlType__::SQL_INSERT, &table });

		if (sqlite3_exec(this->m_data->sq3,
			sql_buff, this->sql_callback, static_cast<void*>(&task), nullptr) != SQLITE_OK)
		{
			return MERROR::SQLITECREATEERR;
		}
		this->m_data->sql_thread.sql_exec_lock = new Mutex();
		this->m_data->sql_thread.sql_exec_th = new Thread(
			[this] {
				
			});

		boost::filesystem::recursive_directory_iterator end;
		// skip permisson denied
		boost::filesystem::recursive_directory_iterator dir_it(Path(sqlite_path), 
			boost::filesystem::directory_options::skip_permission_denied);
		
		while (dir_it != end)
		{
			
		}
		
	}
	int MFileDataBase::constructDataBase(const String& sqlite_path)
	{
		this->constructDataBase(sqlite_path.c_str());
	}

	// get file from base message
	MFileDataBase::FRstream& MFileDataBase::readFile(const char* file_name){}
	MFileDataBase::FRstream& MFileDataBase::readFile(const String& file_name){}
	MFileDataBase::FRstream& MFileDataBase::readFileWithoutFormat(const char* file_name){}
	MFileDataBase::FRstream& MFileDataBase::readFileWithoutFormat(const String& file_name){}

	// write file
	int MFileDataBase::writeFile(const char* file_name, const char* format, Fstream& fstream){}
	int MFileDataBase::writeFile(const String& file_name, const String& format, Fstream& fstream){}

	// earse dir
	int MFileDataBase::earseFile(const char* dir){}
	int MFileDataBase::earseFile(const String& dir){}

	// get Dir like a file from base message
	MFileDataBase::Fstream& MFileDataBase::readDir(const char* dir_name){}
	MFileDataBase::Fstream& MFileDataBase::readDir(const String& dir_name){}

	// write Dir like a file from base message
	int MFileDataBase::writeDir(const char* dir_name, const char* format, int file_count, int file_size){}
	int MFileDataBase::writeDir(const String& dir_name, const String& format, Fstream& fstream){}

	// earse dir
	int MFileDataBase::earseDir(const char* dir){}
	int MFileDataBase::earseDir(const String& dir){}
}