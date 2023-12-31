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
		void example_1()
		{
			MMAVReader reader;
			if (reader.open("E:/迅雷下载/[231203][231124][ピンクパイナップル]となりの家のアネットさん THE ANIMATION 第2巻(No Watermark).mp4") < 0)
			{
				printf("Open File Fail\n");
				reader.close();
				return;
			}

			while (true)
			{
				MMAVPackage pack;
				int ret = 0;
				ret = reader.read(pack);
				if (ret < 0)
				{
					reader.close();
					return;
				}
				printf("Read Packet Success, now size is %d, now stream size is %d \n", ret, reader.getStreamSize());
				std::this_thread::sleep_for(std::chrono::microseconds(500));
			}
			reader.close();
		}
		void example_2()
		{
			MMAVFrame frame;
			MMAVPackage package;
			MMAVDecoder decoder;
			MMAVReader reader;

			if (reader.open("E:/迅雷下载/[231203][231124][ピンクパイナップル]となりの家のアネットさん THE ANIMATION 第2巻(No Watermark).mp4") < 0)
			{
				printf("Open File Fail\n");
				reader.close();
				return;
			}
			MMAVStream stream;
			reader.getStream(stream, 0);
			while (true)
			{
				MMAVPackage pack;
				int ret = 0;
				ret = reader.read(pack);
				if (ret < 0)
				{
					reader.close();
					return;
				}
				printf("Read Packet Success, now size is %d, now stream size is %d \n", ret, reader.getStreamSize());

				std::this_thread::sleep_for(std::chrono::microseconds(500));
			}
			reader.close();
		}
	};
}

#endif // !__MUZI_MMAVEXAMPLE_H__
