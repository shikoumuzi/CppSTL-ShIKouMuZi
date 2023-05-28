#ifndef __MUZI_MFILE_H__
#define __MUZI_MFILE_H__
#include"../MBase/MObjectBase.h"
#include<string>
namespace MUZI
{
	class MFile
	{
	public:
		using String = std::string;
	public:
		struct __MFile_Data__;
	public:
		MFile();
		MFile(const MFile&);
		MFile(MFile&&);
		~MFile();
	public:
		// 文件打开
		int open(char* file_name, char* flag, char* encoding);
		int close();
	public:
		int read();
		int readline();
		int readlines();

		int read_no_lock();
		int readline_no_lock();
		int readlines_no_lock();
	public:
		int write();
		int writeline();
		int writelines();

		int write_no_lock();
		int writeline_no_lock();
		int writelines_no_lock();
	public:
		int seek();
	public:
		int flock();
	public:
		int flush();
	public:
		int is_EOF();
	public:
		struct __MFile_Data__* m_data;

	};
}

#endif // !__MUZI_MFILE_H__
