#ifndef __MUZI_FILEIOQUEUE_H__
#define __MUZI_FILEIOQUEUE_H__
#include"../MBase/MObjectBase.h"
#include<atomic>
#include<stdio.h>
#include"MFile.h"
#include"MFileInput.h"
#include"MFileOutput.h"
namespace MUZI
{
	// 一个队列绑定一个文件
	class FileIOQueue
	{
	public:
		FileIOQueue();
		FileIOQueue(const FileIOQueue&) = delete;
		FileIOQueue(FileIOQueue&&);

	public:
		
		

	public:
		std::atomic<int> read_index;
		std::atomic<int> write_start_index;
		std::atomic<int> write_end_index;
	};
}


#endif // !__MUZI_FILEIOQUEUE_H__

