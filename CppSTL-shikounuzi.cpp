﻿// CppSTL-shikounuzi.cpp: 定义应用程序的入口点。
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
//void SyncAnnularQueueTest()
//{
//	MUZI::MSyncAnnularQueue<int> queue;
//	std::thread t1(
//		[&queue]()
//		{
//			for (int i = 0; i < 1000000000; ++i)
//				queue.push(i);
//		});
//	std::thread t2(
//		[&queue]()
//		{
//			for (int i = 0; i < 1000000; ++i)
//			{
//				int* p = queue.front();
//				if (p != nullptr)
//				{
//					std::cout << std::this_thread::get_id() << ":" << *p << "\n";
//				}
//				queue.pop();
//			}
//		}
//	);
//	//std::thread t3(
//	//	[&queue]()
//	//	{
//	//		for (int i = 0; i < 10000; ++i)
//	//		{
//	//			int* p = queue.front();
//	//			if (p != nullptr)
//	//			{
//	//				std::cout << std::this_thread::get_id() << ":" << *p << "\n";
//	//			}
//	//			std::this_thread::sleep_for(std::chrono::seconds(1));
//	//		}
//	//	}
//	//);
//
//	t1.join();
//	t2.join();
//	//t3.join();
//}
//
//void shared_ptr_test()
//{
//	//int* p = new int(10086);
//	//std::shared_ptr<int> ptr(p);
//	//
//	//std::cout << p << "\n" << &ptr << "\n";
//
//	MUZI::MSyncAnnularQueue<std::shared_ptr<int>> queue;
//	for (int i = 0; i < 1000; ++i)
//	{
//		queue.push(std::shared_ptr<int>(new int(1000)));
//	}
//}

void myprint()
{
	std::vector<int> a;
	return;
}

void myprint(int a, int b, int c)
{
	printf("%d %d %d", a, b, c);
}

template<typename T>
void emit(T t)
{
	std::cout << t << " "; 
}

//template<typename T, typename... Arg>
//void emit(T t, Arg... arg)
//{
//	if (sizeof...(arg) > 0)
//	{
//		std::cout << t << " ";
//		emit(arg...);
//	}
//	else
//	{
//		std::cout << t << " ";
//		emit(arg...);
//	}
//}

template<typename... Arg>
void emit(Arg... arg)
{
	myprint(arg...);
	//printf("just extend arg");
	//if (sizeof...(arg) > 0)
	//{
	//	std::cout << arg << " ";
	//	emit(arg...);
	//}
	//else
	//{
	//	std::cout << arg << " ";
	//	emit(arg...);
	//}
}


void address(const int& a)
{
	std::cout << "fun a:" << &a << std::endl;
}

#include"MMedia/MMPlayer/MMAV/MMAV.h"
#include<thread>
int main(int arg, char* argv[])
{
	//std::cout << MUZI::__muzi_span_stl_type__<std::string> << std::endl;
	//std::string str = "test_str";
	//MUZI::MSpan<std::string> a(str);
	//for (auto x : a)
	//{
	//	std::cout << x ;
	////}
	//MUZI::MTimer* timer = MUZI::MTimer::getMTimer();
	//MUZI::CTask task = timer->set_task(uint32_t(200));
	//timer->start_task(task);
	//int i = 0;
	//while (timer->call_task(task))
	//{
	//	std::cout << "i: " << i++ << std::endl;
	//}
	std::cout << "hello world " << std::endl;
	//server();

	//const char8_t* s = u8"text string";
	//std::string data = "dfsfdndlskfjldksjfklsd";
	//char r[] = "dasjjfdsjflksdfjlkdsfjlsdkfja[opjpgjnglkfmglkfmgs[";
	//MUZI::MLog::w("MLog::w Text", "error message is %s\n", data.c_str());
	//MUZI::MLog::w("MLog::w Text", "error message is %s\n", s);
	//MUZI::MLog::w("MLog::w Text", "error message is %s\n", r);
	//fprintf(stdout, "fprintf Text: :error message is %s\n", data.c_str());
	//fprintf(stdout, "fprintf Text: :error message is %s\n", s);
	//fprintf(stdout, "fprintf Text: :error message is %s\n", r);

	//async_server();

	//SyncAnnularQueueTest();

	//shared_ptr_test();

	//emit(1, 2, 2);
	//int a = 0;
	//std::cout << "main a:" << &a << std::endl;
	//address(a);

	MUZI::ffmpeg::MMAVReader reader;
	if (reader.open("E:/迅雷下载/[231203][231124][ピンクパイナップル]となりの家のアネットさん THE ANIMATION 第2巻(No Watermark).mp4") < 0)
	{
		printf("Open File Fail\n");  
		reader.close();
		return -1;
	}

	while (true)
	{
		MUZI::ffmpeg::MMAVPackage pack;
		int ret = 0;
		ret = reader.read(pack);
		if (ret < 0)
		{
			reader.close();
			return -1;
		}
		printf("Read Packet Success, now size is %d\n", ret);
		std::this_thread::sleep_for(std::chrono::microseconds(500));

	}
	reader.close();
	return 0;


}
