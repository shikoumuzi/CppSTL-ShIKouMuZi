#pragma once
#ifndef __MUZI_MMAV_EXAMPLE_H__
#define __MUZI_MMAV_EXAMPLE_H__
#include"../MMAV.h"
#include<thread>
#include<vector>
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
			if (reader.open("E:/儻跡和墮/[231203][231124][ピンクパイナップル]となりの社のアネットさん THE ANIMATION 及2��(No Watermark).mp4") < 0)
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
			MMAVReader reader;
			MMAVPackage packet;

			std::vector<MMAVDecoder> decoder_list;

			if (reader.open("E:/儻跡和墮/[231203][231124][ピンクパイナップル]となりの社のアネットさん THE ANIMATION 及2��(No Watermark).mp4") < 0)
			{
				printf("Open File Fail\n");
				reader.close();
				return;
			}

			for (int i = 0; i < reader.getStreamSize(); ++i)
			{
				auto stream = reader.getStream(i);
				auto decoder = MMAVDecoder();
				if ((decoder.initDecoder(stream)) != 0)
				{
					fprintf(stderr, "Init Decoder failed");
					return;
				}
				decoder_list.emplace_back(std::move(decoder));
			}
			int ret = 0;
			while (true)
			{
				MMAVPackage pack;
				ret = reader.read(pack);
				if (ret < 0)
				{
					reader.close();
					return;
				}
				printf("Read Packet Success, now size is %d, now stream size is %d \n", ret, reader.getStreamSize());
				// 壓寄方楚議stream扮��decoder鰯協議stream才pack嶄議stream_index尖哘�猴�
				auto& decoder = decoder_list[pack.getStreamIndex()];
				ret = decoder.sendPackage(pack);

				if (ret)
				{
					continue;
				}

				while (ret != MERROR::MAV_DECODER_RECV_FRAME_EOF)
				{
					MMAVFrame frame;
					ret = decoder.recvFrame(frame);
					if (ret)
					{
						break;
					}

					// success to do
				}
				// 壓盾裂頼穎朔匆俶勣序佩賠茅複噫産贋pack議荷恬
				for (int i = 0; i < decoder_list.size(); ++i)
				{
					decoder_list[i].clearBuffer();
				}
			}
			reader.close();
		}
	};
}

#endif // !__MUZI_MMAVEXAMPLE_H__
