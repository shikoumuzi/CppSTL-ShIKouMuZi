#pragma once
#ifndef __MUZI_MMAVDECODERMANAGER_H__
#define __MUZI_MMAVDECODERMANAGER_H__
#include<map>
#include"MMAVDecoder.h"
namespace MUZI::ffmpeg
{
	struct MMAVDecoderWrapper
	{
		size_t& stream_index;
	};

	class MMAVDecoderManager
	{
	public:
		using Map = std::map<MMAVDecoderWrapper, MMAVDecoder>;
	public:
		int addDecoder();
		int eraseDecoder();

	private:
		Map m_decoder_mapping;
	};
}

#endif // !__MUZI_MMAVDECODERMANAGER_H__
