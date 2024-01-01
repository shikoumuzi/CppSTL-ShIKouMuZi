#include"MMAVStream.h"
namespace MUZI::ffmpeg
{
	MMAVStream::MMAVStream() :
		m_stream_index(-1),
		m_code_parm(avcodec_parameters_alloc())
	{
	}

	MMAVStream::MMAVStream(int stream_index, AVCodecParameters* code_parm) :
		m_stream_index(stream_index),
		m_code_parm(code_parm)
	{
	}

	MMAVStream::MMAVStream(const MMAVStream& stream) :
		m_stream_index(stream.m_stream_index),
		m_code_parm(avcodec_parameters_alloc())
	{
		avcodec_parameters_copy(this->m_code_parm, stream.m_code_parm);
	}

	MMAVStream::MMAVStream(MMAVStream&& stream) :
		m_stream_index(stream.m_stream_index),
		m_code_parm(stream.m_code_parm)
	{
		stream.m_code_parm = nullptr;
		stream.m_stream_index = -1;
	}

	void MMAVStream::operator=(const MMAVStream& stream)
	{
		this->m_stream_index = stream.m_stream_index;
		avcodec_parameters_copy(this->m_code_parm, stream.m_code_parm);
	}

	void MMAVStream::operator=(MMAVStream&& stream)
	{
		this->m_stream_index = stream.m_stream_index;
		this->m_code_parm = stream.m_code_parm;
		stream.m_code_parm = nullptr;
		stream.m_stream_index = -1;
	}

	inline int MMAVStream::getStreamIndex()
	{
		return this->m_stream_index;
	}

	bool MMAVStream::isVaild()
	{
		return this->m_code_parm == nullptr;
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