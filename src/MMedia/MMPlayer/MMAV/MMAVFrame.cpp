#include "MMAVFrame.h"

namespace MUZI::ffmpeg
{
	MMAVFrame::MMAVFrame() :
		m_av_frame(av_frame_alloc())
	{
	}

	MMAVFrame::MMAVFrame(const MMAVFrame& av_frame) :
		MMAVFrame()
	{
		av_frame_copy(this->m_av_frame, av_frame.m_av_frame);
	}

	MMAVFrame::MMAVFrame(MMAVFrame&& av_frame) :
		m_av_frame(av_frame.m_av_frame)
	{
		av_frame.m_av_frame = nullptr;
	}

	MMAVFrame::~MMAVFrame()
	{
		if (this->m_av_frame != nullptr)
		{
			av_frame_free(&this->m_av_frame);
			this->m_av_frame = nullptr;
		}
	}
}