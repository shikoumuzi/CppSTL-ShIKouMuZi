#pragma once

#ifndef __MUZI_MPOLYMORPHIC_FUNCTIONAL_H__
#define __MUZI_MPOLYMORPHIC_FUNCTIONAL_H__
#include<memory>
template<typename ReturnType, typename... Args>
class MPolymorphicFunction
{
public:
	MPolymorphicFunction() {
	}
	MPolymorphicFunction(const MPolymorphicFunction& func) {
	}
	MPolymorphicFunction(MPolymorphicFunction&& func) {
	}
	template<typename Lambda>
	MPolymorphicFunction(const Lambda& lambda) :
		m_handler(std::make_shared<Handler>(lambda))
	{}
	~MPolymorphicFunction() {
	}
public:
	ReturnType operator()(Args... args) {
		this->m_handler->invoke();
	}
private:
	class HandlerBase {
	public:
		virtual ReturnType invoke(Args...) const = 0;
		virtual ~HandlerBase() = 0;
	};

	template<typename Func>
	class Handler : public HandlerBase {
	public:
		using return_type = typename std::enable_if<std::is_same_v<typename std::invoke_result<Func>::type, ReturnType>, ReturnType>::type;
	public:
		Handler() : m_func()
			Handler(Func&& func) : m_func(func) {}
		Handler(const Func& func) : m_func(func) {}
	public:
		ReturnType invoke(Args... args) {
			return std::invoke(this->m_func, args);
		}
	public:
		Func m_func;
	};

private:
	std::shared_ptr<HandlerBase> m_handler;
};

#endif // !__MUZI_MPOLYMORPHIC_FUNCTIONAL_H__
