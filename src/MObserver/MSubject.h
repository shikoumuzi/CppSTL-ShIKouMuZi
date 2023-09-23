#pragma once
#ifndef __MUZI_MOBSERVER_MSUBJECT_H__
#define __MUZI_MOBSERVER_MSUBJECT_H__
#include<list>
#include<memory>
#include<string>

namespace MUZI::observer
{
	template<typename T>
	class MObserver;

	template<typename T>
	class MSubject
	{
	public:
		MSubject() : m_state(0) {}

		virtual ~MSubject() = default;

		void attach(const std::shared_ptr<MObserver<T>> pObserver)
		{
			this->m_pObserver_list.push_back(pObserver);
		}

		void detach(const std::shared_ptr<MObserver<T>> pObserver)
		{
			this->detach(pObserver->name());
		}

		void detach(const std::string& name)
		{
			for (auto it = this->m_pObserver_list.begin(); it != this->m_pObserver_list.end(); ++it)
			{
				if ((*it)->name() == name)
				{
					this->m_pObserver_list.erase(it);
					break;
				}
			}
		}

		virtual void notifyWithArg()
		{
			for (auto it = this->m_pObserver_list.begin(); it != this->m_pObserver_list.end(); ++it)
			{
				(*it)->update(this->m_subject_data);
			}
		}

		virtual void notify()
		{
			for (auto it = this->m_pObserver_list.begin(); it != this->m_pObserver_list.end(); ++it)
			{
				(*it)->update();
			}
		}

		virtual int getState() { return m_state; }

		void setState(int state) {
			m_state = state;
		}

		void setSubjectData(T data)
		{
			this->m_subject_data = data;
			this->notifyWithArg(this->m_subject_data)
		}

		const T& getSubjectDataByRef()
		{
			return this->m_subject_data;
		}

		T getSubjectData()
		{
			return this->m_subject_data;
		}

	protected:
		std::list<std::shared_ptr<MObserver>> m_pObserver_list;
		int m_state;
		T m_subject_data;
	};
}

#endif // !__MUZI_MOBSERVER_MSUBJECT_H__
