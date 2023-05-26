#include"MFileDataBase.h"
#include<cstring>
#include<sqlite3.h>

namespace MUZI
{
	struct MFileDataBase::__MFileDataBase_Data__
	{
		const char* root;
	
	};

	MFileDataBase::MFileDataBase():m_data(new struct __MFileDataBase_Data__)
	{
		if (!boost::filesystem::exists(Path("./sqlite")))
		{
			
		}
	}
	MFileDataBase::MFileDataBase(MFileDataBase&& that)noexcept
	{

	}
	MFileDataBase::~MFileDataBase()
	{

	}

	// bind a root dir to construct an file database
	int MFileDataBase::bind(const char* root)
	{
		if (boost::filesystem::is_directory(Path(root)))
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
		
	}
	int MFileDataBase::constructDataBase(const String& sqlite_path){}

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