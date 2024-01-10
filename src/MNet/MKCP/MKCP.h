#pragma once
#ifndef __MUZI_MKCP_H__
#define __MUZI_MKCP_H__
#include"../MReliableUDP/MReliableUDP.h"
namespace MUZI::net
{
	template<
		bool TimeOut = true, // 超时机制
		bool CRC = true, // CRC验证
		bool HandshakeAndWave = true, // 三次握手四次挥手 将不采用连接池而是采用将信息包含在前三次握手当中，让client 自述第几次
		bool Ordered = true,  // 保证包排序
		bool Retransfer = true, // 重传机制
		size_t BUFFER = 8192 // 缓冲池
	>
	using MKCP = MReliableUDP<TimeOut, CRC, HandshakeAndWave, Ordered, Retransfer, BUFFER>;
}

#endif // !__MUZI_MKCP_H__
