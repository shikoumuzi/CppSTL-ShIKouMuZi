#pragma once
#ifndef __MUZI_MEVENTOBJECT_H__
#define __MUZI_MEVENTOBJECT_H__
#include"MEvent.h"
namespace MUZI::_event
{
	/// @brief 这是指包含有event事件处理函数的根类
	class MEventObject
	{
	public:
		virtual void event(const MEvent& event)
		{}
	};
}

#endif 