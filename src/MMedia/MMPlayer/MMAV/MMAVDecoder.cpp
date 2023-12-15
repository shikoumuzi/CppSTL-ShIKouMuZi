#include"MMAVDecoder.h"
namespace MUZI::ffmpeg
{
	MMAVDecoder::MMAVDecoder() :
		m_av_codec_context(avcodec_alloc_context3(nullptr))
	{
	}

	MMAVDecoder::MMAVDecoder(const MMAVDecoder& decoder) :
		MMAVDecoder()
	{
		avcodec_copy_context(this->m_av_codec_context, decoder.m_av_codec_context);
	}

	MMAVDecoder::MMAVDecoder(MMAVDecoder&& decoder) :
		m_av_codec_context(decoder.m_av_codec_context)
	{
		decoder.m_av_codec_context = nullptr;
	}

	MMAVDecoder::~MMAVDecoder()
	{
		if (this->m_av_codec_context != nullptr)
		{
			avcodec_free_context(&this->m_av_codec_context);
			this->m_av_codec_context = nullptr;
		}
	}
	int MMAVDecoder::initDecoder()
	{
		return 0;
	}
}