#pragma once
#ifndef __MUZI_MMAV_EXAMPLE_H__
#define __MUZI_MMAV_EXAMPLE_H__
#include"../MMAV.h"
#include<thread>
namespace MUZI::ffmpeg::example
{
	class MMAVExample
	{
	public:
		MMAVExample() {}
	public:
		int example()
		{
			MMAVReader reader;
			if (reader.open("E:/Ñ¸À×ÏÂÔØ/[231203][231124][¥Ô¥ó¥¯¥Ñ¥¤¥Ê¥Ã¥×¥ë]¤È¤Ê¤ê¤Î¼Ò¤Î¥¢¥Í¥Ã¥È¤µ¤ó THE ANIMATION µÚ2Ž†(No Watermark).mp4") < 0)
			{
				printf("Open File Fail\n");
				reader.close();
				return -1;
			}

			while (true)
			{
				MMAVPackage pack;
				int ret = 0;
				ret = reader.read(pack);
				if (ret < 0)
				{
					reader.close();
					return -1;
				}
				printf("Read Packet Success, now size is %d, now stream size is %d \n", ret, reader.getStreamSize());
				std::this_thread::sleep_for(std::chrono::microseconds(500));
			}
			reader.close();
		}
	};
}

#endif // !__MUZI_MMAVEXAMPLE_H__
