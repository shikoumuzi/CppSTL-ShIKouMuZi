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
			return -1;
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
			return -1;
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
		return this->m_av_format_context->nb_streams;
	}

	int MMAVReader::getStream(MMAVStream& stream, int stream_id)
	{
		if (stream_id >= this->m_av_format_context->nb_streams)
		{
			return MERROR::STREAM_ID_NO_EXIT;
		}
		stream.m_stream_index = this->m_av_format_context->streams[stream_id]->index;
		return 0;
	}
}