#pragma once
#ifndef __MUZI_MRELIABLEUDP_H__
#define __MUZI_MRELIABLEUDP_H__

namespace MUZI::net
{
	template<
		bool TimeOut = true, // ��ʱ����
		bool CRC = true, // CRC��֤
		bool HandshakeAndWave = true, // ���������Ĵλ��� �����������ӳض��ǲ��ý���Ϣ������ǰ�������ֵ��У���client �����ڼ���
		bool Ordered = true,  // ��֤������
		bool Retransfer = true, // �ش�����
		size_t BUFFER = 8192 // �����
	>
	class MReliableUDP
	{
	public:
		static void getMReliableUDPServer()
		{}
		static void getMReliableUDPClient()
		{}
	public:
		class MReliableUDPServer
		{};
		class MReliableUDPClient
		{};
	public:

	public:
		int send()
		{
		}
	};
}

#endif