#pragma once
#ifndef __MUZI_MMAVSTREAM_H__
#define __MUZI_MMAVSTREAM_H__
#include"MMAVBase.h"

namespace MUZI::ffmpeg
{
	class MMAVStream
	{
		MMAV_FRIEND_CLASS
	public:
		MMAVStream();
		MMAVStream(const MMAVStream&);
		MMAVStream(MMAVStream&&);
		~MMAVStream();
	private:
		MMAVStream(int stream_index, AVCodecParameters* m_code_parm);
	public:
		void operator=(const MMAVStream& stream);
		void operator=(MMAVStream&& stream);
		inline int getStreamIndex();
	private:
		int m_stream_index;
		AVCodecParameters* m_code_parm;
	};
}
#endif // !__MUZI_MMAVSTREAM_H__
