#pragma once
#ifndef __MUZI_MNETBASE_H__
#define __MUZI_MNETBASE_H__
#include<string>
#include<boost/asio.hpp>
#include<boost/asio/io_context.hpp>
namespace MUZI::NET
{
	using String = std::string;
	using Address = boost::asio::ip::address;
	using EndPoint = boost::asio::ip::tcp::endpoint;
	using EC = boost::system::error_code;
	using AddressSeccion = boost::asio::ip::address_v4;
	using Protocol = boost::asio::ip::tcp;
	using TCPSocket = boost::asio::ip::tcp::socket;
	using IOContext = boost::asio::io_context;
	using TCPAcceptor = boost::asio::ip::tcp::acceptor;
}


#endif // !__MUZI_MNETBASE_H__
