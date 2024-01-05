#include"MMAVEncoder.h"

namespace MUZI::ffmpeg
{
	MMAVEncoder::MMAVEncoder():
		m_av_codec_context(avcodec_alloc_context3(nullptr))
	{
	}

	MMAVEncoder::MMAVEncoder(const MMAVEncoder& encoder):
		MMAVEncoder()
	{
		avcodec_copy_context(this->m_av_codec_context, encoder.m_av_codec_context);
	}

	MMAVEncoder::MMAVEncoder(MMAVEncoder&& encoder):
		m_av_codec_context(encoder.m_av_codec_context)
	{
		encoder.m_av_codec_context = nullptr;
	}

	MMAVEncoder::~MMAVEncoder()
	{
		if (this->m_av_codec_context != nullptr)
		{
			avcodec_free_context(&this->m_av_codec_context);
			this->m_av_codec_context = nullptr;
		}
	}
	int MMAVEncoder::initEncoder()
	{
		return 0;
	}
	int MMAVEncoder::closeEncoder()
	{
		return 0;
	}
	int MMAVEncoder::sendFrame(const MMAVFrame& frame)
	{
		avcodec_send_frame(this->m_av_codec_context, frame.m_av_frame);
		return 0;
	}
	int MMAVEncoder::recvPackage(MMAVPackage& package)
	{
		avcodec_receive_packet(this->m_av_codec_context, package.m_av_packet);

		return 0;
	}
	MMAVPackage MMAVEncoder::recvPackage(int& error_number)
	{
		auto package = MMAVPackage();
		error_number = this->recvPackage(package);
		return package;
	}
}