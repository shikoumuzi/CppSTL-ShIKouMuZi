#include"MFileDataBase.h"
#include<cstring>
#include<sqlite3.h>
#include <boost/algorithm/string.hpp>
namespace MUZI
{

	struct MFileDataBase::__MFileDataBase_Data__
	{
		char* root;
		sqlite3* sq3;
		sqlite3_stmt* sql_select;
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

	MFileDataBase::MFileDataBase(const char* sqlite_dir_path) :m_data(new struct __MFileDataBase_Data__({nullptr, nullptr, nullptr}))
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
				sqlite3_close_v2(this->m_data->sq3);
				this->m_data->sq3 = nullptr;
			}
			if (this->m_data->root != nullptr)
			{
				delete this->m_data->root;
				this->m_data->root = nullptr;
			}
			if (this->m_data->sql_select != nullptr)
			{
				sqlite3_finalize(this->m_data->sql_select);
				this->m_data->sql_select = nullptr;
			}
			delete this->m_data;
			this->m_data = nullptr;
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
			this->m_data->root = new char[path_name_len];
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
	int MFileDataBase::constructDataBase() 
	{
		int err_msg = 0;
		char tablename[20] = { __MUZI_MFILEDATABASE_SQL_TABLE_NAME__ };
		const char* sqlite_path = "";

		Path table_path(this->m_data->root);
		table_path /= strcat(tablename, ".db");
		
		// 打开sqlite3
		if ((err_msg = sqlite3_open_v2(table_path.string().c_str(), &this->m_data->sq3, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE, nullptr)) != SQLITE_OK
			&& !sqlite3_db_readonly(this->m_data->sq3, strcat(tablename, ".db")))
		{
			fprintf(stderr, "sqlite3_open_v2 is error, errmsg is %s", sqlite3_errmsg(this->m_data->sq3));
			return MERROR::SQLITE_OPEN_ERR;
		}

		// 创建表单
		__MFileDataBase_Sql_Task__ task({ __SqlType__::SQL_CREATE, 0, nullptr});
		if (err_msg =(sqlite3_exec(this->m_data->sq3, "", this->sql_callback, static_cast<void*>(&task), nullptr)) != SQLITE_OK)
		{
			fprintf(stderr, "sqlite3_exec is error, errmsg is %s", sqlite3_errmsg(this->m_data->sq3));
			return MERROR::SQLITE_CREATETABLE_ERR;
		}

		
		__MFileDataBase_Sql_Page__ table({0, 0});
		task = { __SqlType__::SQL_INSERT, 0, &table };

		boost::filesystem::recursive_directory_iterator end;
		// skip permisson denied
		boost::filesystem::recursive_directory_iterator dir_it(Path(sqlite_path),
			boost::filesystem::directory_options::skip_permission_denied);

		// 构建sql语句预编译
		sqlite3_stmt* p_stmt = nullptr;
		const char* sql = "INSERT INTO FILE_MSG(filename, filepath) VALUES(?,?);";
		if (err_msg = sqlite3_prepare_v2(this->m_data->sq3, sql, strlen(sql), &p_stmt, nullptr) != SQLITE_OK)
		{
			fprintf(stderr, "sqlite3_prepare_v2 is error, errmsg is %s", sqlite3_errmsg(this->m_data->sq3));
			return MERROR::SQLITE_PREPARE_ERR;
		}
		
		// 遍历文件目录的同时插入表单 
		int nCol = 1;
		while (dir_it != end)
		{
			nCol = 1;
			if (boost::filesystem::is_regular_file(dir_it->path()))
			{
				// SQLITE_TRANSIENT 表示sqlite会内部复制一份字符串并在适当的时候释放
				// -1表示其自动计算长度
				// stem表示去除扩展名后的内容
				sqlite3_bind_text(p_stmt, nCol++, dir_it->path().stem().string().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(p_stmt, nCol, dir_it->path().string().c_str(), -1, SQLITE_TRANSIENT);
				// 执行语句
				sqlite3_step(p_stmt);
				sqlite3_reset(p_stmt);
			}
		}
		// 删除“插入stmt”
		sqlite3_finalize(p_stmt);

		// 更换数据库打开模式为只读模式
		sqlite3_close(this->m_data->sq3);
		if ((err_msg = sqlite3_open_v2(sqlite_path, &this->m_data->sq3, SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE, nullptr)) != SQLITE_OK)
		{
			fprintf(stderr, "sqlite3_open is error, errcode is %d", err_msg);
			return MERROR::SQLITE_OPEN_ERR;
		}

		// 编译查询语句
		std::vector<std::string> v{ "SELECT * FROM ", static_cast<const char*>(__MUZI_MFILEDATABASE_SQL_TABLE_NAME__ ) , " WHERE filename=?" };
		std::string sql_select = boost::algorithm::join(v, "");
		if ((err_msg = sqlite3_prepare_v2(this->m_data->sq3, sql_select.c_str(), sql_select.size() + 1, &this->m_data->sql_select, nullptr)) != SQLITE_OK)
		{
			fprintf(stderr, "sqlite3_open is error, errcode is %d", err_msg);
			return MERROR::SQLITE_PREPARE_ERR;
		}
		
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