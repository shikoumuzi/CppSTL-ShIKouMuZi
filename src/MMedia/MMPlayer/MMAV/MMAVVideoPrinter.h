#pragma once
#ifndef __MUZI_MMAVVIDEOPRINTER_H__
#define __MUZI_MMAVVIDEOPRINTER_H__
#include"MMAVFrame.h"
#include"MMAVBase.h"
#include"MSTL/h/MArray.h"

extern "C"
{
#include<libavutil/pixdesc.h>
}

namespace MUZI::ffmpeg
{
	class MMAVVideoPrinter
	{
		MMAV_FRIEND_CLASS;
	public:
		MMAVVideoPrinter();

	public:
		int bind(MMAVFrame& frame);
	private:
		int m_width;
		int m_height;
		MHeapArray<char, 128> m_pix_format_string;
		AVPixelFormat m_format;
	};
}

#endif // !__MUZI_MMAVVIDEOPRINTER_H__
