#ifndef __MUZI_FILEIOQUEUE_H__
#include"../MBase/MObjectBase.h"
#include<atomic>
#include<stdio.h>
#include"MFile.h"
#include"MFileInput.h"
#include"MFileOutput.h"
namespace MUZI
{
	class FileIOQueue
	{
	public:
		FileIOQueue();
		FileIOQueue(const FileIOQueue&) = delete;
		FileIOQueue(FileIOQueue&&);

	public:
		
		

	public:
		std::atomic<int> read_index;
		std::atomic<int> write_index;
	};
}


#endif // !__MUZI_FILEIOQUEUE_H__

