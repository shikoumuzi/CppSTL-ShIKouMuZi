#pragma once
#ifndef __MUZI_MKCP_H__
#define __MUZI_MKCP_H__
#include"../MReliableUDP/MReliableUDP.h"
namespace MUZI::net
{
	template<
		bool TimeOut = true, // ��ʱ����
		bool CRC = true, // CRC��֤
		bool HandshakeAndWave = true, // ���������Ĵλ��� �����������ӳض��ǲ��ý���Ϣ������ǰ�������ֵ��У���client �����ڼ���
		bool Ordered = true,  // ��֤������
		bool Retransfer = true, // �ش�����
		size_t BufferSize = 8192 // �����
	>
	using MKCP = MReliableUDP<TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, BufferSize>;
}

#endif // !__MUZI_MKCP_H__
