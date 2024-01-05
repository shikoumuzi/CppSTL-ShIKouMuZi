#pragma once
#ifndef __MUZI_MMAVDCODER_H__
#define __MUZI_MMAVDCODER_H__
#include"MMAVBase.h"
#include"MMAVStream.h"
#include"MMAVPackage.h"
#include"MMAVFrame.h"

namespace MUZI::ffmpeg
{
	class MMAVDecoder
	{
		MMAV_FRIEND_CLASS
	public:
		MMAVDecoder();
		MMAVDecoder(const MMAVDecoder&);
		MMAVDecoder(MMAVDecoder&&);
		~MMAVDecoder();
	public:
		int initDecoder(MMAVStream& av_stream);
		int closeDecoder();
	public:
		int sendPackage(const MMAVPackage& pkt); 
		int recvFrame(MMAVFrame& frm);
		MMAVFrame recvFrame(int& error_number);
	public:
		void clearBuffer();
	private:
		size_t m_stream_index;
		AVCodecContext* m_av_codec_context;
	};
}

#endif // !__MUZI_MMAVDCODER_H__
