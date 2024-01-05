#include"MMAVPackage.h"
namespace MUZI::ffmpeg
{
	MMAVPackage::MMAVPackageRef::MMAVPackageRef() :
		m_av_packet(av_packet_alloc())
	{}
	MMAVPackage::MMAVPackageRef::MMAVPackageRef(const MMAVPackageRef& package) :
		m_av_packet(av_packet_alloc())
	{
		av_packet_ref(this->m_av_packet, package.m_av_packet);
	}
	MMAVPackage::MMAVPackageRef::MMAVPackageRef(MMAVPackageRef&& package) :
		m_av_packet(package.m_av_packet)
	{
		package.m_av_packet = nullptr;
	}
	MMAVPackage::MMAVPackageRef::~MMAVPackageRef()
	{
		if (this->m_av_packet != nullptr)
		{
			// av_packet_free 会自动减少引用计数
			av_packet_free(&this->m_av_packet);
		}
	}

	void MMAVPackage::MMAVPackageRef::operator=(MMAVPackage& package)
	{
		if (this->m_av_packet == nullptr)
		{
			this->m_av_packet = av_packet_alloc();
		}
		else
		{
			av_packet_unref(this->m_av_packet);
		}
		this->m_av_packet = package.getRef().m_av_packet;
	}

	void MMAVPackage::MMAVPackageRef::operator=(MMAVPackageRef& package)
	{
		if (this->m_av_packet == nullptr)
		{
			this->m_av_packet = av_packet_alloc();
		}
		else
		{
			av_packet_unref(this->m_av_packet);
		}
		av_packet_ref(this->m_av_packet, package.m_av_packet);
	}

	MMAVPackage::MMAVPackage() :
		m_av_packet(av_packet_alloc())
	{}
	MMAVPackage::MMAVPackage(MMAVPackage&& package) :
		m_av_packet(package.m_av_packet)
	{
		package.m_av_packet = av_packet_alloc();
	}
	MMAVPackage::~MMAVPackage()
	{
		if (this->m_av_packet != nullptr)
		{
			av_packet_free(&this->m_av_packet);
			this->m_av_packet = nullptr;
		}
	}
	void MMAVPackage::operator=(const MMAVPackage& package)
	{
		this->m_av_packet = av_packet_clone(package.m_av_packet);
	}
	MMAVPackage::MMAVPackageRef MMAVPackage::getRef()
	{
		auto ret_pack = MMAVPackageRef();
		av_packet_ref(ret_pack.m_av_packet, this->m_av_packet);
		return ret_pack;
	}
	size_t MMAVPackage::getStreamIndex()
	{
		return this->m_av_packet->stream_index;
	}
	const MMAVPackage::MMAVPackageRef MMAVPackage::getRef() const
	{
		auto ret_pack = MMAVPackageRef();
		av_packet_ref(ret_pack.m_av_packet, this->m_av_packet);
		return ret_pack;
	}
	size_t MMAVPackage::getStreamIndex() const
	{
		return this->m_av_packet->stream_index;
	}
}