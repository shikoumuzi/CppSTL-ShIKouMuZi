#ifndef __MUZI__MFILEDATABASE_H__
#define __MUZI__MFILEDATABASE_H__
#include<boost/filesystem.hpp>
#include<boost/filesystem/fstream.hpp>
#include<boost/system/error_code.hpp>
#include<string.h>
#include"MBase/MError.h"
namespace MUZI
{

	class MFileDataBase
	{
	public:
		using String = std::string;
		using Fstream = boost::filesystem::fstream;
		using Path = boost::filesystem::path;
		using GetFileStaus = boost::filesystem::file_status(*)(Path const &);
		using BError = boost::system::error_code;
		static GetFileStaus getFileStatus;

	private:
		struct __MFileDataBase_Data__;
	public:
		MFileDataBase();
		MFileDataBase(const MFileDataBase&) = delete;
		MFileDataBase(MFileDataBase&&)noexcept;
		~MFileDataBase();
	public:
		// bind a root dir to construct an file database
		int bind(const char* root);
		int bind(const String& root);
		
		// create file database base on the binding message
		int constructDataBase(const char* sqlite_path);
		int constructDataBase(const String& sqlite_path);

		// get file from base message
		Fstream& readFile(const char* file_name);
		Fstream& readFile(const String& file_name);
		Fstream& readFileWithoutFormat(const char* file_name);
		Fstream& readFileWithoutFormat(const String& file_name);
	
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
		
		struct __MFileDataBase_Data__* m_data;
	};
};


#endif // !__MUZI__MFILEDATABASE_H__


