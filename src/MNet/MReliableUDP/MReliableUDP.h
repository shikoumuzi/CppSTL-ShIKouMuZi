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
		bool buffer = true // �����
	>
	class MReliableUDP
	{
	public:
		void getMReliableUDPServer()
		{}
		void getMReliableUDPClient()
		{}
	public:
		class MReliableUDPServer
		{};
		class MReliableUDPClient
		{};
	public:
	};
}

#endif