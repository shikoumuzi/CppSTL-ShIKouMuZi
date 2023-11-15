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
	private:
		static std::once_flag _flag;
	public:
		//template<typename... Args>
		//static std::shared_ptr<T> initInstance(Args args)
		//{
		//	std::call_once(MSingleton::_flag, [this]() {
		//		MSingleton::_instance = std::make_shared<T>(args...);
		//		});
		//	return MSingleton::_instance;
		//}
		static std::shared_ptr<T> getInstance()
		{
			std::call_once(MSingleton::_flag, [this]() {
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

