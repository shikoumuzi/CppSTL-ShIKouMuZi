#pragma once
#ifndef __MUZI_MEVENTLOOP_H__
#define __MUZI_MEVENTLOOP_H__
#include"MEvent.h"
#include<memory>
namespace MUZI::_event
{
	class MEventLoop
	{
	public:
		void push_back(std::shared_ptr<MEvent> event)
		{

		}


	};
}
#endif // !__MUZI_MEVENTLOOP_H__
