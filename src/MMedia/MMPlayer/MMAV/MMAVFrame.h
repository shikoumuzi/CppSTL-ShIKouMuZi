#pragma once
#ifndef __MUZI_MMAVFRAME_H__
#define __MUZI_MMAVFRAME_H__
#include"MMAVBase.h"
namespace MUZI::ffmpeg
{
	class MMAVFrame
	{
		MMAV_FRIEND_CLASS
	public:
		MMAVFrame();
		MMAVFrame(const MMAVFrame& av_frame);
		MMAVFrame(MMAVFrame&& av_frame);
		~MMAVFrame();
	private:
		AVFrame* m_av_frame;
	};
}

#endif // !__MUZI_MMAVFRAME_H__
