#include"MFileDataBase.h"
#include<cstring>
#include<sqlite3.h>

namespace MUZI
{

	struct MFileDataBase::__MFileDataBase_Data__
	{
		char* root;
		char* sqlite_dir;
		char* sqlite_table;
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
		int offset;
		void* data;
	};

	struct __MFileDataBase_Sql_Callback__
	{
	public:
		static int SQL_CREATE(__MFileDataBase_Sql_Task__* para, char** columnValue)
		{

		}
		static int SQL_DELETE(__MFileDataBase_Sql_Task__* para, char** columnValue)
		{

		}
		// SELECT 对于结构体内容的内存分配不做查看，只管写入， 并且通过table_size确定当前条数，pageid确定page数组位置
		static int SQL_SELECT(__MFileDataBase_Sql_Task__* para, char** columnValue)
		{
			struct __MFileDataBase_Sql_Page__* page
				= static_cast<struct __MFileDataBase_Sql_Page__*>(
					static_cast<struct __MFileDataBase_Sql_Page__**>(para->data)[para->offset]);
			page->tables[page->table_szie++] = { columnValue[1], columnValue[2]};
		}
		static int SQL_INSERT(__MFileDataBase_Sql_Task__* para, char** columnValue)
		{

		}
	public:
		static int(*sql_callback[4])(__MFileDataBase_Sql_Task__* para, char** columnValue);
		using Sql_Callback = int(*[4])(__MFileDataBase_Sql_Task__* para, char** columnValue);
	};
	
	__MFileDataBase_Sql_Callback__::Sql_Callback __MFileDataBase_Sql_Callback__::sql_callback
		= { __MFileDataBase_Sql_Callback__::SQL_CREATE, 
			__MFileDataBase_Sql_Callback__::SQL_DELETE,
			__MFileDataBase_Sql_Callback__::SQL_INSERT,
			__MFileDataBase_Sql_Callback__::SQL_SELECT,};
	

	int MFileDataBase::sql_callback(void* para, int columenCount, char** columnValue, char** columnName)
	{
		struct __MFileDataBase_Sql_Task__* data = static_cast<struct __MFileDataBase_Sql_Task__*>(para);

		return __MFileDataBase_Sql_Callback__::sql_callback[data->type](data, columnValue);
	}

	MFileDataBase::GetFileStaus MFileDataBase::getFileStatus = boost::filesystem::status;

	MFileDataBase::MFileDataBase(const char* sqlite_dir_path) :m_data(new struct __MFileDataBase_Data__({nullptr, nullptr, nullptr, nullptr}))
	{
		// 获取应用程序所在路径
		Path initial_path = boost::filesystem::initial_path<boost::filesystem::path>();
		boost::filesystem::current_path(initial_path);// 设置工作路径为当前应用程序所在目录
		
		Path path(sqlite_dir_path);
		// canonical 可以去除相对路径中的.. 以防止非法攻击
		path = boost::filesystem::canonical(initial_path / path);
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
		this->__delete__();
		
	}

	void MFileDataBase::__delete__()
	{
		if (this->m_data != nullptr)
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
			if (this->m_data->sql_thread.sql_exec_cond != nullptr)
			{
				this->m_data->sql_thread.sql_exec_cond->notify_all();
				delete this->m_data->sql_thread.sql_exec_cond;
			}
			if (this->m_data->sql_thread.sql_exec_lock != nullptr)
			{
				this->m_data->sql_thread.sql_exec_lock->unlock();
				delete this->m_data->sql_thread.sql_exec_lock;
			}
			delete this->m_data;
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
		char sql_buff[256] = { "File_Sql_\0"};
		if (sqlite3_open(sqlite_path, &this->m_data->sq3) )
		{
			return MERROR::SQLITEOPENERR;
		}

		strcpy(sql_buff, strcat(sql_buff, Path(this->m_data->root).filename().string().c_str()));
		__MFileDataBase_Sql_Task__ task({ __SqlType__::SQL_CREATE, 0, nullptr});
		if (sqlite3_exec(this->m_data->sq3, sql_buff, this->sql_callback, static_cast<void*>(&task), (char**)&err_msg) != SQLITE_OK)
		{

		}

		__MFileDataBase_Sql_Page__ table({0, 0});
		task = { __SqlType__::SQL_INSERT, 0, &table };

		if (sqlite3_exec(this->m_data->sq3,
			sql_buff, this->sql_callback, static_cast<void*>(&task), nullptr) != SQLITE_OK)
		{
			return MERROR::SQLITECREATEERR;
		}

		struct __MFileDataBase_Data__* m_data = this->m_data;
		this->m_data->sql_thread.sql_exec_lock = new Mutex();
		this->m_data->sql_thread.sql_exec_th = new Thread(
			[this, m_data] {
				while(m_data->sql_thread.THREAD_WORK)
				{

				}


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