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
		size_t package_size; // ����С
		size_t data_size; // �������ݴ�С
		size_t package_type : 2; // ������
		size_t hand_shake_times : 3; // ���ִ���
		size_t wave_times : 3; // ���ִ���
	};
	struct MReliableUDPRegisterPackage : public MReliableUDPPackage
	{
		MReliableUDPIdentifier identifier; // ��ʶ�鿴�Ƿ�Ϊͬһ��ģ�����ɵ�Server��Client
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
		bool TimeOut = true, // ��ʱ����
		bool CRC = true, // CRC��֤
		bool HandshakeAndWave = true, // ���������Ĵλ��� �����������ӳض��ǲ��ý���Ϣ������ǰ�������ֵ��У���client �����ڼ���
		bool Ordered = true,  // ��֤������
		bool Retransfer = true, // �ش�����
		bool SlidingWindow = true, // ��������
		size_t BufferSize = 8192 // ����ش�С
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
		bool TimeOut, // ��ʱ����
		bool CRC, // CRC��֤
		bool HandshakeAndWave, // ���������Ĵλ��� �����������ӳض��ǲ��ý���Ϣ������ǰ�������ֵ��У���client �����ڼ���
		bool Ordered,  // ��֤������
		bool Retransfer, // �ش�����
		bool SlidingWindow, // ��������
		size_t BufferSize // ����ش�С
	>
	MReliableUDPIdentifier MReliableUDP<TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, SlidingWindow, BufferSize>::MReliableUDPServer::\
		identifier = { TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, SlidingWindow, BufferSize };
	template<
		bool TimeOut, // ��ʱ����
		bool CRC, // CRC��֤
		bool HandshakeAndWave, // ���������Ĵλ��� �����������ӳض��ǲ��ý���Ϣ������ǰ�������ֵ��У���client �����ڼ���
		bool Ordered,  // ��֤������
		bool Retransfer, // �ش�����
		bool SlidingWindow, // ��������
		size_t BufferSize // ����ش�С
	>
	const char MReliableUDP<TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, SlidingWindow, BufferSize>::MReliableUDPServer::\
		eof[__MUZI_MRELIABLEUDP_SIZEOF_UDP_EOF_IN_BYTE__] = "\0\r\n";
}

#endif