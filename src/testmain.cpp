#include<iostream>
#include"MAllocator.h"
#include"MSpan.h"
#include<string>
#include<vector>
#include<chrono>
#include"MTimer.h"
class a
{

};

int main()
{
	//std::cout << MUZI::__muzi_span_stl_type__<std::string> << std::endl;
	//std::string str = "test_str";
	//MUZI::MSpan<std::string> a(str);
	//for (auto x : a)
	//{
	//	std::cout << x ;
	//}
	MUZI::MTimer* timer = MUZI::MTimer::getMTimer();
	MUZI::CTask task = timer->set_task(uint32_t(200));
	timer->start_task(task);
	int i = 0;
	while (timer->call_task(task))
	{
		std::cout << "i: " << i++ << std::endl;
	}


}