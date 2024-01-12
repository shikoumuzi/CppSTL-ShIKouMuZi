#pragma once
#ifndef __MUZI_MRELIABLEUDP_H__
#define __MUZI_MRELIABLEUDP_H__
#include<array>
#include<memory>
#include<boost/asio.hpp>
#include"MNet/MNetBase.h"
#define __MUZI_MRELIABLEUDP_SIZEOF_UDP_DATA_IN_BYTE__ 1400
#define __MUZI_MRELIABLEUDP_SIZEOF_UDP_EOF_IN_BYTE__ 6

namespace MUZI::net
{
	struct MReliableUDPIdentifier
	{
		bool timeout : 1;
		bool crc : 1;
		bool hand_shake_and_wave : 1;
		bool ordered : 1;
		bool retransfer : 1;
		bool sliding_windows : 1;
	};
	struct MReliableUDPPackage
	{
		size_t package_size; // 包大小
		size_t data_size; // 包内数据大小
		size_t package_type : 2; // 包类型
		size_t hand_shake_times : 3; // 握手次数
		size_t wave_times : 3; // 挥手次数
	};
	struct MReliableUDPRegisterPackage : public MReliableUDPPackage
	{
		MReliableUDPIdentifier identifier; // 标识查看是否为同一个模板生成的Server和Client
	};
	struct MReliableUDPDataPackage : public MReliableUDPPackage
	{
		char data[__MUZI_MRELIABLEUDP_SIZEOF_UDP_DATA_IN_BYTE__];
	};
	struct MReliableUDPEndingPackage : public MReliableUDPPackage
	{
		char eof[__MUZI_MRELIABLEUDP_SIZEOF_UDP_EOF_IN_BYTE__];
	};

	template<
		bool TimeOut = true, // 超时机制
		bool CRC = true, // CRC验证
		bool HandshakeAndWave = true, // 三次握手四次挥手 将不采用连接池而是采用将信息包含在前三次握手当中，让client 自述第几次
		bool Ordered = true,  // 保证包排序
		bool Retransfer = true, // 重传机制
		bool SlidingWindow = true, // 滑动窗口
		size_t BufferSize = 8192 // 缓冲池大小
	>
	class MReliableUDP
	{
	public:

	public:
		class MReliableUDPServer
		{
		public:
			static Identifier identifier;
			static const char eof[__MUZI_MRELIABLEUDP_SIZEOF_UDP_EOF_IN_BYTE__];
		public:
			using Buffer = std::array<char, BufferSize>;
		public:
			int bind()
			{}
			int listen()
			{}
			int accept()
			{}
		public:
			int send()
			{}
			int receive()
			{}
			int read()
			{}
			int write()
			{}
		private:
			int analyizeRegisterCode()
			{
			}
		private:
			bool isMatchedClient()
			{
			}
			int getTimeOutResult()
			{
				constexpr if (TimeOut == true)
				{
				}
				return 0;
			}
			int getCRCResult()
			{
				constexpr if (CRC == true)
				{
				}
				return 0;
			}
			int getHandShakeAndWaveResult()
			{
				constexpr if (HandshakeAndWave == true)
				{
				}
				return 0;
			}
			int getOrderdResult()
			{
				constexpr if (Ordered == true)
				{
				}
				return 0;
			}
			int getRetransferResult()
			{
				constexpr if (Retransfer == true)
				{
				}
				return 0;
			}
			int getSlidingWindowResult()
			{
				constexpr if (SlidingWindow == true)
				{
				}
				return 0;
			}
		private:
			Buffer m_send_buffer;
			int m_back_log;
		};
		class MReliableUDPClient
		{
		public:
			int connect()
			{}
		public:
			int send()
			{}
			int receive()
			{}
			int read()
			{}
			int write()
			{}
		public:
		};
	public:
		static std::shared_ptr<MReliableUDPServer> getMReliableUDPServer()
		{
			return std::make_shared<MReliableUDPServer>();
		}
		static std::shared_ptr<MReliableUDPClient> getMReliableUDPClient()
		{
			return std::make_shared<MReliableUDPClient>();
		}
	};

	template<
		bool TimeOut, // 超时机制
		bool CRC, // CRC验证
		bool HandshakeAndWave, // 三次握手四次挥手 将不采用连接池而是采用将信息包含在前三次握手当中，让client 自述第几次
		bool Ordered,  // 保证包排序
		bool Retransfer, // 重传机制
		bool SlidingWindow, // 滑动窗口
		size_t BufferSize // 缓冲池大小
	>
	MReliableUDPIdentifier MReliableUDP<TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, SlidingWindow, BufferSize>::MReliableUDPServer::\
		identifier = { TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, SlidingWindow, BufferSize };
	template<
		bool TimeOut, // 超时机制
		bool CRC, // CRC验证
		bool HandshakeAndWave, // 三次握手四次挥手 将不采用连接池而是采用将信息包含在前三次握手当中，让client 自述第几次
		bool Ordered,  // 保证包排序
		bool Retransfer, // 重传机制
		bool SlidingWindow, // 滑动窗口
		size_t BufferSize // 缓冲池大小
	>
	const char MReliableUDP<TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, SlidingWindow, BufferSize>::MReliableUDPServer::\
		eof[__MUZI_MRELIABLEUDP_SIZEOF_UDP_EOF_IN_BYTE__] = "\0\r\n";
}

#endif