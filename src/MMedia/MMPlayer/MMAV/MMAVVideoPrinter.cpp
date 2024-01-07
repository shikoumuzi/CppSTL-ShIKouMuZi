#include"MMAVVideoPrinter.h"

namespace MUZI::ffmpeg
{
	int MMAVVideoPrinter::bind(MMAVFrame& frame)
	{
		this->m_format = static_cast<AVPixelFormat>(frame.m_av_frame->format);
		av_get_pix_fmt_string(this->m_pix_format_string.data(), this->m_pix_format_string.size(), this->m_format);

		return 0;
	}
}