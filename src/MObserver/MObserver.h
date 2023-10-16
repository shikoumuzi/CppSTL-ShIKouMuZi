#pragma once
#ifndef __MUZI_MOBSERVER_H__
#define __MUZI_MOBSERVER_H__
#include<memory>
#include<string>
#include"MSubject.h"
#include<iostream>
namespace MUZI::observer
{
	template<typename T>
	class __declspec(novtable) MObserver
	{
	public:
		friend class MSubject<T>;
	public:
		virtual ~MObserver() = default;

		MObserver(const std::weak_ptr<MSubject> pSubject, const std::string& name = "unknown")
			: m_pSubject(pSubject), m_strName(name) {}

		virtual void update() = 0;
		virtual void update(const T& ele) = 0;

		virtual const std::string& name() { return m_strName; }

	protected:
		std::weak_ptr<MSubject> m_pSubject;
		std::string m_strName;
	};

}

#endif // !__MUZI_MOBSERVER_H__
