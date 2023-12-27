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
		class MMAVFrameRef
		{
			MMAV_FRIEND_CLASS
		private:
			MMAVFrameRef();
		public:
			MMAVFrameRef(const MMAVFrameRef& av_frame);
			MMAVFrameRef(MMAVFrameRef&& av_frame);
			~MMAVFrameRef();
		public:
			void operator=(MMAVFrame& av_frame);
			void operator=(MMAVFrameRef& av_frame);
		private:
			AVFrame* m_av_frame;
		};
		using ref = MMAVFrameRef;
	public:
		MMAVFrame();
		MMAVFrame(const MMAVFrame& av_frame);
		MMAVFrame(MMAVFrame&& av_frame);
		~MMAVFrame();
	public:
		MMAVFrameRef getRef();
	public:
		void operator=(const MMAVFrame& av_frame);
	private:
		AVFrame* m_av_frame;
	};
}

#endif // !__MUZI_MMAVFRAME_H__
