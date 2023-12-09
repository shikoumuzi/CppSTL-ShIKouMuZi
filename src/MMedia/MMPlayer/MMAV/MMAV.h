#pragma once
#ifndef __MUZI_MMAV_H__
#define __MUZI_MMAV_H__
#include<boost/filesystem.hpp>
namespace MUZI::mav
{
	class MMAVReader
	{
	public:
		using Path = boost::filesystem::path;
	public:
		MMAVReader();
		~MMAVReader();
	public:
		int open(const Path& file_path);
		int close();
		int read();
	};
}

#endif // !__MUZI_MMAV_H__
