#pragma once
#ifndef __MUZI_MOBSERVER_MSUBJECT_H__
#define __MUZI_MOBSERVER_MSUBJECT_H__
#include<list>
#include<memory>
namespace MUZI::observer
{
	class __declspec(novtable) MObserver;
	class MSubject
	{
	public:
		MSubject() : m_nState(0) {}

		virtual ~MSubject() = default;

		virtual void Attach(const std::shared_ptr<MObserver> pObserver) = 0;

		virtual void Detach(const std::shared_ptr<MObserver> pObserver) = 0;

		virtual void Notify() = 0;

		virtual int GetState() { return m_nState; }

		void SetState(int state) {
			m_nState = state;
		}

	protected:
		std::list<std::shared_ptr<MObserver>> m_pObserver_list;
		int m_nState;
	};
}

#endif // !__MUZI_MOBSERVER_MSUBJECT_H__
