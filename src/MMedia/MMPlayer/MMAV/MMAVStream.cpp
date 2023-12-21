#include"MMAVStream.h"
namespace MUZI::ffmpeg
{
	MMAVStream::MMAVStream():
		m_stream_index(-1),
		m_code_parm(avcodec_parameters_alloc())
	{
	}

	MMAVStream::MMAVStream(const MMAVStream& stream):
		m_stream_index(stream.m_stream_index),
		m_code_parm(avcodec_parameters_alloc())
	{
		avcodec_parameters_copy(this->m_code_parm, stream.m_code_parm);
	}

	MMAVStream::MMAVStream(MMAVStream&& stream):
		m_stream_index(stream.m_stream_index),
		m_code_parm(stream.m_code_parm)
	{
		stream.m_code_parm = nullptr;
		stream.m_stream_index = -1;
	}

	MMAVStream::~MMAVStream()
	{
		if (this->m_code_parm != nullptr)
		{
			avcodec_parameters_free(&this->m_code_parm);
			this->m_code_parm = nullptr;
		}
	}
}