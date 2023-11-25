#pragma once
#ifndef __MUZI_MRELIABLEUDP_H__
#define __MUZI_MRELIABLEUDP_H__

namespace MUZI::net
{
	template<
		bool TimeOut = true,
		bool CRC = true,
		bool HandshakeAndWave = true,
		bool Ordered = true,
		bool Retransfer = true,
		bool buffer = true
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