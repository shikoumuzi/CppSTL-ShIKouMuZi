#pragma once
#ifndef __MUZI_MOBSERVER_H__
#define __MUZI_MOBSERVER_H__
#include<memory>
#include<string>
#include"MSubject.h"
#include<iostream>
namespace MUZI::observer
{


	class __declspec(novtable) MObserver
	{
	public:
		virtual ~MObserver() = default;

		MObserver(const std::shared_ptr<MSubject> pSubject, const std::string& name = "unknown")
			: m_pSubject(pSubject), m_strName(name) {}

		virtual void Update() = 0;

		virtual const std::string& name() { return m_strName; }

	protected:
		std::shared_ptr<MSubject> m_pSubject;
		std::string m_strName;

	};

}

#endif // !__MUZI_MOBSERVER_H__
