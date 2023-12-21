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
		decoder.m_av_codec_context = avcodec_alloc_context3(nullptr);
	}

	MMAVDecoder::~MMAVDecoder()
	{
		if (this->m_av_codec_context != nullptr)
		{
			avcodec_free_context(&this->m_av_codec_context);
			this->m_av_codec_context = nullptr;
		}
	}
	int MMAVDecoder::initDecoder(MMAVStream& av_stream)
	{
		// 拷贝解码器信息到流
		avcodec_parameters_to_context(this->m_av_codec_context, av_stream.m_code_parm);
		auto p_av_codec = avcodec_find_decoder(this->m_av_codec_context->codec_id);

		if (p_av_codec == nullptr)
		{
			return MERROR::MAV_DECODER_NOT_FOUND;
		}

		int ret = avcodec_open2(this->m_av_codec_context, p_av_codec, nullptr);
		if (ret < 0)
		{
			return MERROR::MAV_DECODER_OPEN_FAILED;
		}

		return 0;
	}

	int MMAVDecoder::sendPackage(MMAVPackage& pkt)
	{
		int ret = avcodec_send_packet(this->m_av_codec_context, pkt.m_av_packet);
		if (ret < 0)
		{
			return (ret == AVERROR_EOF) ? MERROR::MAV_DECODER_SEND_EOF : MERROR::MAV_DECODER_SEND_PACKAGE_FAILED;
		}
		return 0;
	}

	int MMAVDecoder::recvPackage(MMAVPackage& pkt)
	{
		return 0;
	}
}