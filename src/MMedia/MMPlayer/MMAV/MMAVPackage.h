#pragma once
#ifndef __MUZI_MMAVPACKAGE_H__
#define __MUZI_MMAVPACKAGE_H__
#include<boost/filesystem.hpp>
#include"MMAVBase.h"

namespace MUZI::ffmpeg
{
	class MMAVPackage
	{
		MMAV_FRIEND_CLASS
	public:
		friend class MMAVReader;
	public:
		class MMAVPackageRef
		{
			MMAV_FRIEND_CLASS
		private:
			MMAVPackageRef();
		public:
			MMAVPackageRef(const MMAVPackageRef& package);
			MMAVPackageRef(MMAVPackageRef&& package);
			~MMAVPackageRef();
		public:
			void operator=(MMAVPackage& package);
			void operator=(MMAVPackageRef& package);
		private:
			AVPacket* m_av_packet;
		};
		using ref = MMAVPackageRef;
	public:
		MMAVPackage();
		MMAVPackage(MMAVPackage&& package);
		~MMAVPackage();
	public:
		void operator=(const MMAVPackage& package);
		void operator=(const MMAVPackageRef& package);
	public:
		MMAVPackageRef getRef();
		size_t getStreamIndex();
	public:
		const MMAVPackageRef getRef() const;
		size_t getStreamIndex() const;
	private:
		AVPacket* m_av_packet;
	};
}

#endif // !__MUZI_MMAVPACKAGE_H__
