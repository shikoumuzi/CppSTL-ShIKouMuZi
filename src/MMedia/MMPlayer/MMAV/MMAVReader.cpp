#include"MMAVPackage.h"
#include"MMAVReader.h"
#include<libavformat/avio.h>

namespace MUZI::ffmpeg
{
	MMAVReader::MStreamIterator::MStreamIterator() :
		m_stream(nullptr),
		parent(nullptr)
	{
	}
	MMAVReader::MStreamIterator::MStreamIterator(const MStreamIterator& iterator) :
		m_stream(std::make_unique<MMAVStream>(parent->getStream(iterator.m_stream->getStreamIndex()))),
		parent(iterator.parent)
	{
	}
	MMAVReader::MStreamIterator::MStreamIterator(MStreamIterator&& iterator) :
		m_stream(std::move(iterator.m_stream)),
		parent(iterator.parent)
	{
	}
	MMAVReader::MStreamIterator::MStreamIterator(int stream_id, MMAVReader* parent) :
		m_stream(std::make_unique<MMAVStream>(parent->getStream(stream_id))),
		parent(parent)
	{
	}
	MMAVReader::MStreamIterator::~MStreamIterator()
	{
	}
	MMAVReader::MStreamIterator MMAVReader::MStreamIterator::operator+(size_t offset)
	{
		MStreamIterator iterator(*this);
		iterator += offset;
		return iterator;
	}
	void MMAVReader::MStreamIterator::operator+=(size_t offset)
	{
		if (offset + this->m_stream->getStreamIndex() >= this->parent->getStreamSize())
		{
			this->m_stream.reset();
			this->m_stream = nullptr;
		}
		this->m_stream->operator=(this->parent->getStream(this->m_stream->getStreamIndex() + offset));
	}
	MMAVReader::MStreamIterator MMAVReader::MStreamIterator::operator-(size_t offset)
	{
		MStreamIterator iterator(*this);
		iterator -= offset;
		return iterator;
	}
	void MMAVReader::MStreamIterator::operator-=(size_t offset)
	{
		if (offset + this->m_stream->getStreamIndex() >= this->parent->getStreamSize())
		{
			this->m_stream.reset();
			this->m_stream = nullptr;
		}
		if (this->m_stream == nullptr)
		{
			this->m_stream->operator=(this->parent->getStream(this->parent->getStreamSize()));
			offset -= 1;
		}
		this->m_stream->operator=(this->parent->getStream(this->m_stream->getStreamIndex() - offset));
	}
	MMAVStream& MMAVReader::MStreamIterator::operator->()
	{
		return *this->m_stream;
	}
	MMAVStream& MMAVReader::MStreamIterator::operator*()
	{
		return *this->m_stream;
	}
	void MMAVReader::MStreamIterator::operator=(const MStreamIterator& iterator)
	{
		this->m_stream->operator=(*iterator.m_stream);
	}
	bool MMAVReader::MStreamIterator::operator==(const MStreamIterator& iterator)
	{
		if (this->parent != iterator.parent)
		{
			return false;
		}
		if (this->m_stream == nullptr && iterator.m_stream == nullptr)
		{
			return true;
		}
		return this->m_stream->getStreamIndex() == iterator.m_stream->getStreamIndex();
	}
	std::strong_ordering MMAVReader::MStreamIterator::operator<=>(const MStreamIterator& iterator)
	{
		if (this->parent != iterator.parent)
		{
			return std::strong_ordering::less;
		}
		if (iterator.m_stream == nullptr)
		{
			if (this->m_stream == nullptr)
			{
				return std::strong_ordering::equal;
			}
			else
			{
				return std::strong_ordering::less;
			}
		}
		return this->m_stream->getStreamIndex() <=> iterator.m_stream->getStreamIndex();
	}
	const MMAVStream& MMAVReader::MStreamIterator::operator->() const
	{
		return *this->m_stream;
	}
	const  MMAVStream& MMAVReader::MStreamIterator::operator*() const
	{
		return *this->m_stream;
	}
	const MMAVReader::MStreamIterator MMAVReader::MStreamIterator::operator-(size_t offset) const
	{
		MStreamIterator iterator(this->m_stream->getStreamIndex() - offset, this->parent);
		iterator -= offset;
		return iterator;
	}
	const MMAVReader::MStreamIterator MMAVReader::MStreamIterator::operator+(size_t offset) const
	{
		MStreamIterator iterator(this->m_stream->getStreamIndex() + offset, this->parent);
		iterator += offset;
		return iterator;
	}
	bool MMAVReader::MStreamIterator::operator==(const MStreamIterator& iterator) const
	{
		if (this->parent != iterator.parent)
		{
			return false;
		}
		if (this->m_stream == nullptr && iterator.m_stream == nullptr)
		{
			return true;
		}
		return this->m_stream->getStreamIndex() == iterator.m_stream->getStreamIndex();
	}
	std::strong_ordering MMAVReader::MStreamIterator::operator<=>(const MStreamIterator& iterator) const
	{
		if (this->parent != iterator.parent)
		{
			return std::strong_ordering::less;
		}
		if (iterator.m_stream == nullptr)
		{
			if (this->m_stream == nullptr)
			{
				return std::strong_ordering::equal;
			}
			else
			{
				return std::strong_ordering::less;
			}
		}
		return this->m_stream->getStreamIndex() <=> iterator.m_stream->getStreamIndex();
	}

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
			return MERROR::MAV_READER_OPEN_INPUT_FAILED;
		}
		if (avformat_find_stream_info(this->m_av_format_context, nullptr) < 0)
		{
			return MERROR::MAV_READER_FIND_STREAM_FAILED;
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
	MMAVPackage MMAVReader::read(int& error_number)
	{
		MMAVPackage ret_pack;
		error_number = this->read(ret_pack);
		return ret_pack;
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
	MMAVStream MMAVReader::getStream(int stream_id)
	{
		if (stream_id >= this->m_av_format_context->nb_streams)
		{
			return MMAVStream();
		}
		auto stream = MMAVStream();
		auto p_ffmepg_stream = this->m_av_format_context->streams[stream_id];
		stream.m_stream_index = p_ffmepg_stream->index;
		avcodec_parameters_copy(stream.m_code_parm, p_ffmepg_stream->codecpar);
		return stream;
	}
	MMAVStream MMAVReader::operator[](size_t index)
	{
		auto stream = MMAVStream();
		if (index < this->m_av_format_context->nb_streams)
		{
			auto p_ffmepg_stream = this->m_av_format_context->streams[index];
			stream.m_stream_index = p_ffmepg_stream->index;
			avcodec_parameters_copy(stream.m_code_parm, p_ffmepg_stream->codecpar);
		}
		return stream;
	}
	MMAVReader::MStreamIterator MMAVReader::begin()
	{
		if (this->m_av_format_context->nb_streams == 0)
		{
			return this->end();
		}
		return MStreamIterator(0, this);
	}
	MMAVReader::MStreamIterator MMAVReader::end()
	{
		return MStreamIterator();
	}
}