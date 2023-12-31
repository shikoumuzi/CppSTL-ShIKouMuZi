#pragma once
#ifndef __MUZI_MSYNCSOCKET_EXAMPLE_H__
#define __MUZI_MSYNCSOCKET_EXAMPLE_H__
#include"MNet/MSyncSocket/MSyncSocket.h"

namespace MUZI::net::sync::example
{
	class MSyncSocketExample
	{
	public:
		void example()
		{
			MUZI::net::sync::MSyncSocket socket(std::move(MUZI::net::MServerEndPoint(10086)));
			int error_code = 0;
			socket.bind();
			socket.listen();
			std::vector<std::thread> thread_list;
			const int LENGTH = 1024;
			int i = 0;
			while (1)
			{
				MUZI::net::sync::NetSyncIOAdapt adapt = socket.accept(error_code);
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
								std::cout << adapt.get()->remote_endpoint() << ": " << data << std::endl;

								ec = socket.write(adapt, std::string(data));
							}
							}));
				}
			}
		}
	};
}

#endif // !__MUZI_MSYNCSOCKET_EXAMPLE_H__
