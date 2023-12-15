#pragma once
#ifndef __MUZI_MMAVSTREAM_H__
#define __MUZI_MMAVSTREAM_H__
#include"MAVBase.h"
#include<libavcodec/avcodec.h>
extern "C"
{
}
namespace MUZI::ffmpeg
{
	class MMAVStream
	{
	public:
		friend class MMAVReader;
	public:
		MMAVStream();
		MMAVStream(const MMAVStream&);
		MMAVStream(MMAVStream&&);
		~MMAVStream();

	private:
		int m_stream_index = -1;
		AVCodecParameters* m_code_parm;
	};
}
#endif // !__MUZI_MMAVSTREAM_H__
