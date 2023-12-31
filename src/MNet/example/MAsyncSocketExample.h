#pragma once
#ifndef __MUZI_MASYNCSOCKET_EXAMPLE_H__
#define __MUZI_MASYNCSOCKET_EXAMPLE_H__
#include"MNet/MAsyncSocket/MAsyncServer.h"
namespace MUZI::net::async::example
{
	class MAsyncSocketExample
	{
	public:
		void example_1()
		{
			int error_code = 0;
			MUZI::net::async::MAsyncServer server(error_code, MUZI::net::MServerEndPoint(10086));
			if (error_code != 0)
			{
				return;
			}
			server.listen(20);
			//auto adapt = server.accept(error_code);
			int adapt = server.accept(
				[](MUZI::net::async::MAsyncServer& server, MUZI::net::async::NetAsyncIOAdapt adapt)->void
				{
					server.readAllFromSocket(adapt, 100);
				});

			//server.readAllFromeSocket(adapt, 20);

			//auto msg = adapt->getPopFrontRecvMsg();
			/*std::cout << static_cast<char*>(msg->getData()) << std::endl;*/
			//server.wtiteAllToSocket(adapt, "sdada", 5);
			server.run();
		}
	};
}

#endif // !__MUZI_MASYNCSOCKET_EXAMPLE_H__
