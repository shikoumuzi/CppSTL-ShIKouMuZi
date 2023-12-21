// CppSTL-shikounuzi.cpp: 定义应用程序的入口点。
//

//#include "CppSTL-shikounuzi.h"
//#include"MTreeMap.h"
////#include"MRBTree.h"
//#include<iostream>
//#include<vector>
//#include"MFTP/MFTP.h"
//#include"MHashMap.h"
//#include"MFTP/MFTP.h"
//#include"MDeque.h"
//#include"MFileSystem/MFileDataBase.h"
////#include"MSQLite/MSQLiite.h"
//
//#include"MNet/MEndPoint/MClientEndPoint.h"
//#include"MNet/MSyncSocket/MSyncSocket.h"
//#include"MNet/MEndPoint/MServerEndPoint.h"
//#include"MNet/MEndPoint/MClientEndPoint.h"
//#include"MNet/MAsyncSocket/MAsyncServer.h"
//#include<thread>
//#include"MLog/MLog.h"
//#include<MSTL/h/MSyncAnnularQueue.h>

#include<stdio.h>
#include<iostream>
#include<vector>

//void server()
//{
//	MUZI::net::sync::MSyncSocket socket(std::move(MUZI::net::MServerEndPoint(10086)));
//	int error_code = 0;
//	socket.bind();
//	socket.listen();
//	std::vector<std::thread> thread_list;
//	const int LENGTH = 1024;
//	int i = 0;
//	while (1)
//	{
//		MUZI::net::sync::NetSyncIOAdapt adapt = socket.accept(error_code);
//		if (error_code == 0)
//		{
//			thread_list.emplace_back(
//				std::thread([&socket, adapt] {
//					char data[LENGTH];
//					while (1)
//					{
//						memset(data, '\0', LENGTH);
//
//						int ec = socket.read(adapt, data, 1024, true);
//						if (ec != 0)
//						{
//							break;
//						}
//						std::cout << adapt.get()->remote_endpoint() << ": " << data  << std::endl;
//
//						ec = socket.write(adapt, std::string(data));
//					}
//
//					}));
//
//		}
//
//
//	}
//
//}
//
//void async_server()
//{
//	int error_code = 0;
//	MUZI::net::async::MAsyncServer server(error_code, MUZI::net::MServerEndPoint(10086));
//	if (error_code != 0)
//	{
//		return;
//	}
//	server.listen(20);
//	//auto adapt = server.accept(error_code);
//	int adapt = server.accept(
//		[](MUZI::net::async::MAsyncServer& server, MUZI::net::async::NetAsyncIOAdapt adapt)->void
//		{
//			server.readAllFromeSocket(adapt, 100);
//		});
//
//	//server.readAllFromeSocket(adapt, 20);
//
//	//auto msg = adapt->getPopFrontRecvMsg();
//	/*std::cout << static_cast<char*>(msg->getData()) << std::endl;*/
//	//server.wtiteAllToSocket(adapt, "sdada", 5);
//	server.run();
//}
//

#include<thread>
#include"MMedia/MMPlayer/MMAV/example/MMAVExample.h"
int main(int arg, char* argv[])
{
	{
		MUZI::ffmpeg::example::MMAVExample().example();
	}

	return 0;
}