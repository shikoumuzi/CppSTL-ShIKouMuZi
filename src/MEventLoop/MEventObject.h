#pragma once
#ifndef __MUZI_MEVENTOBJECT_H__
#define __MUZI_MEVENTOBJECT_H__
#include"MEvent.h"
namespace MUZI::_event
{
	/// @brief ����ָ������event�¼��������ĸ���
	class MEventObject
	{
	public:
		virtual void event(const MEvent& event)
		{}
	};
}

#endif 