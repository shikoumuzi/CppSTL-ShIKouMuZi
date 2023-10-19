#pragma once
#ifndef __MUZI_SINGLEACTION_H__
#define __MUZI_SINGLEACTION_H__
#include<memory>
#include<mutex>
namespace MUZI::singleton
{

	template<typename T>
	class MSingleton
	{
	public:
		static std::shared_ptr<T> getInstance()
		{
			static std::once_flag _flag;
			std::call_once(_flag, [this]() {
				MSingleton::_instance = std::make_shared<T>();
				});
			return MSingleton::_instance;
		}
	protected:
		MSingleton() = default;
		MSingleton(const MSingleton<T>&) = delete;
		MSingleton(MSingleton&&) = delete;
	public:
		~MSingleton() {}

	private:
		static std::shared_ptr<T> _instance;

	};
	template<typename T>
	std::shared_ptr<T> MSingleton<T>::_instance = nullptr;
}

#endif // !__MUZI_SINGLEACTION_H__

