#pragma once
#ifndef __MUZI_MMAVPACKAGE_H__
#define __MUZI_MMAVPACKAGE_H__
#include<boost/filesystem.hpp>
#include"MMAVBase.h"

namespace MUZI::ffmpeg
{
	class MMAVPackage
	{
		MMAV_FRIEND_CLASS
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
