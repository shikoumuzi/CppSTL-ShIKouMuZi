#include "MMAVFrame.h"

namespace MUZI::ffmpeg
{
	MMAVFrame::MMAVFrameRef::MMAVFrameRef() :
		m_av_frame(av_frame_alloc())
	{
	}

	MMAVFrame::MMAVFrameRef::MMAVFrameRef(MMAVFrameRef& av_frame) :
		MMAVFrameRef()
	{
		av_frame_ref(this->m_av_frame, av_frame.m_av_frame);
	}

	MMAVFrame::MMAVFrameRef::MMAVFrameRef(MMAVFrameRef&& av_frame) :
		m_av_frame(av_frame.m_av_frame)
	{
		av_frame.m_av_frame = nullptr;
	}

	MMAVFrame::MMAVFrameRef::~MMAVFrameRef()
	{
		if (m_av_frame != nullptr)
		{
			av_frame_free(&this->m_av_frame);
		}
	}

	void MMAVFrame::MMAVFrameRef::operator=(MMAVFrame& av_frame)
	{
		if (this->m_av_frame == nullptr)
		{
			this->m_av_frame = av_frame_alloc();
		}
		else
		{
			av_frame_unref(this->m_av_frame);
		}
		this->m_av_frame = av_frame.getRef().m_av_frame;
	}

	void MMAVFrame::MMAVFrameRef::operator=(MMAVFrameRef& av_frame)
	{
		if (this->m_av_frame == nullptr)
		{
			this->m_av_frame = av_frame_alloc();
		}
		else
		{
			av_frame_unref(this->m_av_frame);
		}
		av_frame_ref(this->m_av_frame, av_frame.m_av_frame);
	}

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

	MMAVFrame::MMAVFrameRef MMAVFrame::getRef()
	{
		auto ref = MMAVFrameRef();
		if (this->m_av_frame == nullptr)
		{
			return ref;
		}
		av_frame_ref(ref.m_av_frame, this->m_av_frame);
		return ref;
	}
	void MMAVFrame::operator=(const MMAVFrame& av_frame)
	{
		if (this->m_av_frame == nullptr)
		{
			this->m_av_frame = av_frame_alloc();
		}
		av_frame_copy(this->m_av_frame, av_frame.m_av_frame);
	}
	void MMAVFrame::operator=(const MMAVFrameRef& av_frame)
	{
		if (this->m_av_frame == nullptr)
		{
			this->m_av_frame = av_frame_alloc();
		}
		av_frame_copy(this->m_av_frame, av_frame.m_av_frame);
	}
}