#pragma once
#ifndef __MUZI_MMAVREADER_H__
#define __MUZI_MMAVREADER_H__

#include"MMAVStream.h"
#include"MMAVBase.h"
extern "C"
{
}
#include"MMAVPackage.h"

namespace MUZI::ffmpeg
{
	class MMAVReader
	{
		MMAV_FRIEND_CLASS
	public:
		using Path = boost::filesystem::path;
	public:
		MMAVReader();
		~MMAVReader();
	public:
		int open(const Path& file_path);
		int close();
		int read(MMAVPackage& package);
	public:
		int getStreamSize();
		int getStream(MMAVStream& dest_stream, int stream_id);

	private:
		AVFormatContext* m_av_format_context;
	};
}
#endif // !__MUZI_MMAVREADER_H__
