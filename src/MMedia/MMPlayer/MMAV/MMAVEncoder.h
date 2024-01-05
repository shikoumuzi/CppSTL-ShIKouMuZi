#pragma once
#ifndef __MUZI_MMAVENCODER_H__
#define __MUZI_MMAVENCODER_H__
#include"MMAVBase.h"
#include"MMAVFrame.h"
#include"MMAVPackage.h"
namespace MUZI::ffmpeg
{
	class MMAVEncoder
	{
		MMAV_FRIEND_CLASS
	public:
		MMAVEncoder();
		MMAVEncoder(const MMAVEncoder& encoder);
		MMAVEncoder(MMAVEncoder&& encoder);
		~MMAVEncoder();
	public:
		int initEncoder();
		int closeEncoder();
	public:
		int sendFrame(const MMAVFrame& frame);
		int recvPackage(MMAVPackage& package);
		MMAVPackage recvPackage(int& error_number);
	private:
		AVCodecContext* m_av_codec_context;
	};
}

#endif // !__MUZI_MMAVENCODER_H__
