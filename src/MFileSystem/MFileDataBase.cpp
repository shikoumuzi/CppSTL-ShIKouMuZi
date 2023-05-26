#include"MFileDataBase.h"
#include<cstring>
#include<sqlite3.h>


namespace MUZI
{
	struct MFileDataBase::__MFileDataBase_Data__
	{
		const char* root;
		sqlite3* sq3;
	};

	MFileDataBase::GetFileStaus MFileDataBase::getFileStatus = boost::filesystem::status;

	MFileDataBase::MFileDataBase() :m_data(new struct __MFileDataBase_Data__({nullptr, nullptr}))
	{
		if (!boost::filesystem::exists(Path(".\\sqlite")))
		{
			boost::filesystem::create_directory(Path(".\\sqlite"));
		}
	}
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
		if (boost::filesystem::is_directory(root, ec))
		{
			this->m_data->root = root;
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
		sqlite3_open(sqlite_path, &this->m_data->sq3);
		boost::filesystem::recursive_directory_iterator(Path(sqlite_path));
	}
	int MFileDataBase::constructDataBase(const String& sqlite_path)
	{
		this->constructDataBase(sqlite_path.c_str());
	}

	// get file from base message
	MFileDataBase::Fstream& MFileDataBase::readFile(const char* file_name){}
	MFileDataBase::Fstream& MFileDataBase::readFile(const String& file_name){}
	MFileDataBase::Fstream& MFileDataBase::readFileWithoutFormat(const char* file_name){}
	MFileDataBase::Fstream& MFileDataBase::readFileWithoutFormat(const String& file_name){}

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