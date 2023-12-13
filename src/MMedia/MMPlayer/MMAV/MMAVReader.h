#pragma once
#ifndef __MUZI_MMAVREADER_H__
#define __MUZI_MMAVREADER_H__
#include<boost/filesystem.hpp>
extern "C"
{
#include<libavformat/avformat.h>
}
#include"MMAVPackage.h"

namespace MUZI::ffmpeg
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
		int read(MMAVPackage& package);
	private:
		AVFormatContext* m_av_context;
	};
}
#endif // !__MUZI_MMAVREADER_H__
