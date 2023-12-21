#pragma once
#ifndef __MUZI_MSYNCANNULARQUEUE_EXAMPLE_H__
#define __MUZI_MSYNCANNULARQUEUE_EXAMPLE_H__
#include<thread>
#include"../h/MSyncAnnularQueue.h"
#include<iostream>
namespace MUZI::msyncannulaqueue::example
{
	class MSyncAnnularQueueExample
	{
	public:
		MSyncAnnularQueueExample() {}
	public:
		int example()
		{
			MUZI::MSyncAnnularQueue<int> queue;
			std::thread t1(
				[&queue]()
				{
					for (int i = 0; i < 1000000000; ++i)
						queue.push(i);
				});
			std::thread t2(
				[&queue]()
				{
					for (int i = 0; i < 1000000; ++i)
					{
						int* p = queue.front();
						if (p != nullptr)
						{
							std::cout << std::this_thread::get_id() << ":" << *p << "\n";
						}
						queue.pop();
					}
				}
			);
			//std::thread t3(
			//	[&queue]()
			//	{
			//		for (int i = 0; i < 10000; ++i)
			//		{
			//			int* p = queue.front();
			//			if (p != nullptr)
			//			{
			//				std::cout << std::this_thread::get_id() << ":" << *p << "\n";
			//			}
			//			std::this_thread::sleep_for(std::chrono::seconds(1));
			//		}
			//	}
			//);

			t1.join();
			t2.join();
			//t3.join();
		}
	};
}
#endif // !__MUZI_MSYNCANNULARQUEUE_EXAMPLE_H__
