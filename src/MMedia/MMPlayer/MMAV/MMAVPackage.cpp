#include"MMAVPackage.h"
namespace MUZI::ffmpeg
{
    MMAVPackage::MMAVPackage():
        m_av_packet(av_packet_alloc())
    {}
    MMAVPackage::MMAVPackage(const MMAVPackage & package):
        m_av_packet(av_packet_clone(package.m_av_packet))
    {
    }
    MMAVPackage::MMAVPackage(MMAVPackage&& package):
        m_av_packet(nullptr)
    {
        av_packet_move_ref(package.m_av_packet, this->m_av_packet);
    }
    MMAVPackage::~MMAVPackage()
    {
        if (this->m_av_packet != nullptr)
        {
            av_packet_free(&this->m_av_packet);
            this->m_av_packet = nullptr;
        }
    }
}