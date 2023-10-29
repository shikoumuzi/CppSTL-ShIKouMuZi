#pragma once
#ifndef __MUZI_MSOLT_H__
#define __MUZI_MSOLT_H__
#include"MEventLoop/MEventLoop.h"
#include"MEventLoop/MEvent.h"
namespace MUZI::_event
{
	template<typename T>
	class MSolt: public MEvent
	{
	public:
		MSolt(MEventLoop& loop) : loop(loop)
		{

		}

	private:
		MEventLoop& loop;
		T obj;

	};
}


#endif // !__MUZI_MSOLT_H__
