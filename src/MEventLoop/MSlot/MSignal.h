#pragma once
#ifndef __MUZI_MSIGNAL_H__
#define __MUZI_MSIGNAL_H__
#include"MEventLoop/MEventLoop.h"
#include"MEventLoop/MEvent.h"
#include"MEventLoop/MSlot/MSlot.h"
#include<vector>
#include<map>
#include<memory>
namespace MUZI::_event
{
	template<typename T, typename SlotCallBack, typename... Args>
	class MSignal
	{
	public:
		template<typename SlotCallBack, typename... Args>
		struct SlotMsg
		{
			std::function<void(Args)> callback;
		};

		
		class MSignalTriggerEvent: public MEvent
		{
		public:
			template<typename T, typename SlotCallBack, typename... Args>
			SignalTriggerEvent(MSignal<T, SlotCallBack, Args>::SlotMsg* _msg, Args... args)
			{
				this->callback = [_msg, args]() {
					_msg.callback(args...);
				}
			}
		private:
			std::function<void()> callback;
		};
	public:
		MSignal(MEventLoop& loop): loop(loop)
		{

		}

	public:
		template<typename Slot>
		void connect(Slot* slot, SlotCallBack callback)
		{
			this->slots.push_back({
				.callback = std::bind(callback, std::placeholders::_1, slot);
				});
		}

		template<typename... Args>
		void emit(Args... args)
		{
			for (auto& slot : this->slots)
			{
				this->loop.push_back(MSignalTriggerEvent<void*, SlotCallBack, Args>(&slot, args...));
			}
		}
	private:
		MEventLoop& loop;
		std::vector<MSignal<T, SlotCallBack, Args>::SlotMsg> slots;
		
	};
}

#endif // !__MUZI_MSIGNAL_H__
