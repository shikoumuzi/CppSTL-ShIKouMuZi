#include"MMAVDecoder.h"
namespace MUZI::ffmpeg
{
	MMAVDecoder::MMAVDecoder() :
		m_av_codec_context(avcodec_alloc_context3(nullptr)),
		m_stream_index(-1)
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
		// ������������Ϣ����
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
		this->m_stream_index = av_stream.getStreamIndex();
		return 0;
	}

	int MMAVDecoder::closeDecoder()
	{
		avcodec_close(this->m_av_codec_context);
		return 0;
	}

	// ��Ϊ���ͺͽ��ղ�һ�������Եȣ�����sendһ��package ���Ի�úü��� frame ����һ������ȡ����
	int MMAVDecoder::sendPackage(const MMAVPackage& pkt)
	{
		if (pkt.getStreamIndex() != this->m_stream_index)
		{
			return MERROR::MAV_DECODER_PACKAGE_NOT_MATCH;
		}
		int ret = avcodec_send_packet(this->m_av_codec_context, pkt.m_av_packet);
		if (ret < 0)
		{
			return (ret == AVERROR_EOF) ? MERROR::MAV_DECODER_SEND_EOF : MERROR::MAV_DECODER_SEND_PACKAGE_FAILED;
		}
		return 0;
	}

	int MMAVDecoder::recvFrame(MMAVFrame& frm)
	{
		int ret = avcodec_receive_frame(this->m_av_codec_context, frm.m_av_frame);
		if (ret < 0)
		{
			return (ret == AVERROR_EOF) ? MERROR::MAV_DECODER_RECV_EOF : MERROR::MAV_DECODER_RECV_PACKAGE_FAILED;
		}
		return 0;
	}
	MMAVFrame MMAVDecoder::recvFrame(int& error_number)
	{
		MMAVFrame frame;
		error_number = this->recvFrame(frame);
		return frame;
	}
	void MMAVDecoder::clearBuffer()
	{
		avcodec_send_packet(this->m_av_codec_context, nullptr);
		AVFrame* frame = av_frame_alloc();

		for (;;)
		{
			if (avcodec_receive_frame(this->m_av_codec_context, frame) == AVERROR_EOF)
			{
				break;
			}
		}
	}
}