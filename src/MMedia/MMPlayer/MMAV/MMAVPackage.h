#pragma once
#ifndef __MUZI_MMAVPACKAGE_H__
#define __MUZI_MMAVPACKAGE_H__
#include<boost/filesystem.hpp>
extern "C"
{
#include<libavformat/avformat.h>
}
namespace MUZI::ffmpeg
{
	class MMAVPackage
	{
	public:
		friend class MMAVReader;
	public:
		MMAVPackage();
		MMAVPackage(const MMAVPackage& package);
		MMAVPackage(MMAVPackage&& package);
		~MMAVPackage();
	private:
		AVPacket* m_av_packet;
	};
}

#endif // !__MUZI_MMAVPACKAGE_H__
