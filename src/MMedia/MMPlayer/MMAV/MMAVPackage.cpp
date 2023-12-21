#include"MMAVPackage.h"
namespace MUZI::ffmpeg
{
	MMAVPackage::MMAVPackage() :
		m_av_packet(av_packet_alloc()),
		m_ref_flag(false)
	{}
	MMAVPackage::MMAVPackage(const MMAVPackage& package) :
		m_av_packet(av_packet_clone(package.m_av_packet)),
		m_ref_flag(false)
	{
	}
	MMAVPackage::MMAVPackage(MMAVPackage&& package) :
		m_av_packet(package.m_av_packet),
		m_ref_flag(package.m_ref_flag)
	{
		package.m_av_packet = av_packet_alloc();
		package.m_ref_flag = false;
	}
	MMAVPackage::~MMAVPackage()
	{
		if (this->m_av_packet != nullptr)
		{
			if (this->m_ref_flag)
			{
				av_packet_unref(this->m_av_packet);
				return;
			}
			av_packet_free(&this->m_av_packet);
			this->m_av_packet = nullptr;
		}
	}
	MMAVPackage MMAVPackage::getRef()
	{
		auto ret_pack = MMAVPackage();
		ret_pack.m_ref_flag = true;
		av_packet_ref(ret_pack.m_av_packet, this->m_av_packet);
		return ret_pack;
	}
}