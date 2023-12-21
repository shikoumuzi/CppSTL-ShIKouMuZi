#include"MMAVPackage.h"
#include"MMAVReader.h"
#include<libavformat/avio.h>

namespace MUZI::ffmpeg
{
	MMAVReader::MMAVReader() :
		m_av_format_context(avformat_alloc_context())
	{
	}
	MMAVReader::~MMAVReader()
	{
		if (this->m_av_format_context == nullptr)
		{
			this->close();
			avformat_free_context(this->m_av_format_context);
			this->m_av_format_context = nullptr;
		}
	}
	int MMAVReader::open(const Path& file_path)
	{
		if (this->m_av_format_context == nullptr)
		{
			return MERROR::MAV_OBJECT_IS_NOT_USEFUL;
		}
		if (avformat_open_input(&this->m_av_format_context, file_path.string().c_str(), nullptr, nullptr) < 0)
		{
			return -1;
		}
		if (avformat_find_stream_info(this->m_av_format_context, nullptr) < 0)
		{
			return -1;
		}

		return 0;
	}
	int MMAVReader::close()
	{
		if (this->m_av_format_context == nullptr)
		{
			return MERROR::MAV_OBJECT_IS_NOT_USEFUL;
		}
		avformat_close_input(&this->m_av_format_context);
		return 0;
	}
	int MMAVReader::read(MMAVPackage& package)
	{
		return (av_read_frame(this->m_av_format_context, package.m_av_packet) < 0) ? -1 : package.m_av_packet->size;
	}

	int MMAVReader::getStreamSize()
	{
		if (this->m_av_format_context == nullptr)
		{
			return MERROR::MAV_OBJECT_IS_NOT_USEFUL;
		}
		return this->m_av_format_context->nb_streams;
	}

	int MMAVReader::getStream(MMAVStream& stream, int stream_id)
	{
		if (this->m_av_format_context == nullptr)
		{
			return MERROR::MAV_OBJECT_IS_NOT_USEFUL;
		}
		if (stream_id >= this->m_av_format_context->nb_streams)
		{
			return MERROR::MAV_STREAM_ID_NO_EXIT;
		}
		auto p_ffmepg_stream = this->m_av_format_context->streams[stream_id];
		stream.m_stream_index = p_ffmepg_stream->index;
		avcodec_parameters_copy(stream.m_code_parm, p_ffmepg_stream->codecpar);
		return 0;
	}
}