#ifndef __MUZI_MOPTIONAL_H__
#define __MUZI_MOPTIONAL_H__
#include<optional>
#include<assert.h>
//#include "MBase/MObjectBase.h"
namespace MUZI {
	using __MDefaultTypeDefine__ = int;
	template<typename T = __MDefaultTypeDefine__>
	class MOptional {
	public:
		using  value_type = std::conditional<!std::is_reference<T>::value, T, std::remove_reference_t<T>*>::type;
	private:
		struct __NontrivialDummyType__ {
			constexpr __NontrivialDummyType__() noexcept {
				// This default constructor is user-provided to avoid zero-initialization when objects are value-initialized.
			}
		};
		struct __MOptionalBase__ {
			union Data
			{
				__NontrivialDummyType__ dummy;
				value_type value;
			}data;
			bool has_value_flag;
		};
	public:
		// 做成一个标志常量
		struct NoneOpt { // no-value state indicator
			struct _Tag {};
			constexpr explicit NoneOpt(_Tag) {}
		};
		struct InPlacement {
			explicit InPlacement() = default;
		};
	public:
		template<typename ...Args>
		static MOptional<T> makeOptional(Args ... args) {
			return MOptional<T>(InPlacement(), args...);
		}
		static MOptional<T> makeNone() {
			return MOptional<T>(NoneOpt(NoneOpt::_Tag()));
		}
		template<typename ...Args>
		static value_type getPoint(value_type value, Args ... args) {
			if (std::is_pointer<value_type>::value) {
				return value;
			}
		}
	private:
		__MOptionalBase__ m_base;

	public:
		static constexpr NoneOpt None = NoneOpt(NoneOpt::_Tag());

	public:
		MOptional() : MOptional(NoneOpt(NoneOpt::_Tag())) {}
		explicit MOptional(NoneOpt) {
			this->m_base.has_value_flag = false;
			new(&this->m_base.data.dummy) __NontrivialDummyType__();
		}
		template<typename... Args>
		MOptional(InPlacement, Args ... args) {
			this->m_base.has_value_flag = true;
			if constexpr (!std::is_reference<T>::value) {
				new(&this->m_base.data.value) value_type(args...);
			}
			else {
				this->m_base.data.value = this->getPoint(args...);
			}
		}
	public:
		T unwrap() {
			assert(this->m_base->has_value_flag == false);
			if constexpr (!std::is_reference<T>::value) {
				return static_cast<value_type>(this->m_base.data);
			}
			else {// 如果是引用那就退化成指针
				return *static_cast<value_type>(this->m_base.data);
			}
		}

		inline bool isNone() {
			return !this->m_base.has_value_flag;
		}
	public:
		void reset() {
			this->m_base.has_value_flag = false;
		}
	public:
		bool operator==(NoneOpt None) {
			return !this->m_base.has_value_flag;
		}
	};
}

#endif // !__MUZI_MOPTIONAL_H__
