#pragma once
#ifndef __MUZI_MNETBASE_H__
#define __MUZI_MNETBASE_H__
#include<string>
#include<cstring>
#include<boost/asio.hpp>
#include<boost/asio/io_context.hpp>
#include<vector>
#include<memory>
#include<iostream>
#include<boost/lockfree/queue.hpp>
#include<boost/lockfree/spsc_queue.hpp>
#include<map>
namespace MUZI::net
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
	using Port = uint16_t;
	using HostQuery = boost::asio::ip::tcp::resolver::query;
	using HostResolver = boost::asio::ip::tcp::resolver;
	using ReadBuff = boost::asio::const_buffer;
	using WriteBuff = boost::asio::mutable_buffer;
	using ReadBuffs = boost::asio::const_buffers_1;
	using WriteBuffs = boost::asio::mutable_buffers_1;


	template<typename K, typename V>
	using Map = std::map<K, V>;
	template<typename T, typename U>
	using LockfreeQueue = boost::lockfree::queue<T, U>;
	template<typename T, typename U>
	using SpecQueue = boost::lockfree::spsc_queue<T, U>;
	template<typename T>
	using Vector = std::vector<T>;
}


#endif // !__MUZI_MNETBASE_H__
