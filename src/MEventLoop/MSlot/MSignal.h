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
		struct SlotMsg
		{
			void* slot;
			SlotCallBack callback;
		};
		template<typename Slot, typename SlotCallBack, typename... Args>
		class MSignalTriggerEvent: public MEvent
		{
		public:
			SignalTriggerEvent(struct MSignal<T, SlotCallBack, Args>::SlotMsg* _msg)
				:m_event_msg(std::make_shared<void*>(static_cast<void*>(_msg), [](void*) {}))
			{

			}
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
				.slot = slot,
				.callback = callback
				});
		}

		template<typename... Args>
		void emit(Args... args)
		{
			for (auto& slot : this->slots)
			{
				this->loop.push_back(MSignalTriggerEvent<Slot, SlotCallBack, Args>(&slot));
			}
		}
	private:
		MEventLoop& loop;
		std::vector<struct MSignal<T, SlotCallBack, Args>::SlotMsg> slots;
		
	};
}

#endif // !__MUZI_MSIGNAL_H__
