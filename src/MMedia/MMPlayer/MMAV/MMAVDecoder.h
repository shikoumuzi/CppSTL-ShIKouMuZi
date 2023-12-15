#pragma once
#ifndef __MUZI_MMAVDCODER_H__
#define __MUZI_MMAVDCODER_H__
#include"MAVBase.h"
extern "C"
{
#include<libavcodec/avcodec.h>
}
namespace MUZI::ffmpeg
{
	class MMAVDecoder
	{
	public:
		MMAVDecoder();
		MMAVDecoder(const MMAVDecoder&);
		MMAVDecoder(MMAVDecoder&&);
		~MMAVDecoder();
	public:
		int initDecoder();
	private:
		AVCodecContext* m_av_codec_context;
	};
}

#endif // !__MUZI_MMAVDCODER_H__
