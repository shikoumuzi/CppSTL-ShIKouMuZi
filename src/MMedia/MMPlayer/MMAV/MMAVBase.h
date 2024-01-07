#pragma once
#ifndef __MUZI_MAVBASE_H__
#define __MUZI_MAVBASE_H__
#include"MBase/MError.h"
#include<iso646.h>

#define MMAV_FRIEND_CLASS	public:\
								friend class MMAVDecoder;\
								friend class MMAVPackage;\
								friend class MMAVReader;\
								friend class MMAVStream;\
								friend class MMAVFrame;\
								friend class MMAVPackageRef;\
								friend class MMAVFrameRef;\
								friend class MMAVEncoder;\
								friend class MMAVVideoPrinter;
extern "C"
{
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
}
#include<boost/filesystem.hpp>

#endif // !__MUZI_MAVBASE_H__
