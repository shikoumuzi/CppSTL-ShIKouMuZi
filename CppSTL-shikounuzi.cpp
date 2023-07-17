// CppSTL-shikounuzi.cpp: 定义应用程序的入口点。
//

#include "CppSTL-shikounuzi.h"
#include"MTreeMap.h"
//#include"MRBTree.h"
#include<iostream>
#include<vector>
#include"MFTP/MFTP.h"
#include"MHashMap.h"
#include"MFTP/MFTP.h"
#include"MDeque.h"
#include"MFileSystem/MFileDataBase.h"
//#include"MSQLite/MSQLiite.h"

#include"MNet/MEndPoint/MClientEndPoint.h"
#include"MNet/MSyncSocket/MSyncSocket.h"
#include"MNet/MEndPoint/MServerEndPoint.h"
#include"MNet/MEndPoint/MClientEndPoint.h"
#include"MNet/MAsyncSocket/MAsyncServer.h"
#include<thread>
#include<iostream>
#include"MLog/MLog.h"

void server()
{
	MUZI::NET::SYNC::MSyncSocket socket(std::move(MUZI::NET::MServerEndPoint(10086)));
	int error_code = 0;
	socket.bind();
	socket.listen();
	std::vector<std::thread> thread_list;
	const int LENGTH = 1024;
	int i = 0;
	while (1)
	{
		MUZI::NET::SYNC::NetSyncIOAdapt adapt = socket.accept(error_code);
		if (error_code == 0)
		{
			thread_list.emplace_back(
				std::thread([&socket, adapt] {
					char data[LENGTH];
					while (1)
					{
						memset(data, '\0', LENGTH);

						int ec = socket.read(adapt, data, 1024, true);
						if (ec != 0)
						{
							break;
						}
						std::cout << adapt.get()->remote_endpoint() << ": " << data  << std::endl;

						ec = socket.write(adapt, std::string(data));
					}

					}));
			
		}


	}

}

void async_server()
{
	int error_code = 0;
	MUZI::NET::ASYNC::MAsyncServer server(error_code, MUZI::NET::MServerEndPoint(10086));
}


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

	const char8_t* s = u8"text string";
	std::string data = "dfsfdndlskfjldksjfklsd";
	char r[] = "dasjjfdsjflksdfjlkdsfjlsdkfja[opjpgjnglkfmglkfmgs[";
	MUZI::MLog::w("MLog::w Text", "error message is %s\n", data.c_str());
	MUZI::MLog::w("MLog::w Text", "error message is %s\n", s);
	MUZI::MLog::w("MLog::w Text", "error message is %s\n", r);
	fprintf(stdout, "fprintf Text: :error message is %s\n", data.c_str());
	fprintf(stdout, "fprintf Text: :error message is %s\n", s);
	fprintf(stdout, "fprintf Text: :error message is %s\n", r);

	


}
