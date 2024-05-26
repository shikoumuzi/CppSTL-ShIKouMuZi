#ifndef __MUZI_MOPTIONAL_H__
#define __MUZI_MOPTIONAL_H__
#include<optional>
#include<assert.h>
#include "MBase/MObjectBase.h"
namespace MUZI {
	template<typename T = __MDefaultTypeDefine__>
	class MOptional {
	public:
		using  value_type = std::conditional<std::is_reference<T>::value, T, std::reference_wrapper<T>>::type;
	private:
		struct __NontrivialDummyType__ {
			constexpr __NontrivialDummyType__() noexcept {
				// This default constructor is user-provided to avoid zero-initialization when objects are value-initialized.
			}
		};
		struct MOptionalBase {
			union Data
			{
				__NontrivialDummyType__ dummy;
				value_type value;
			}data;
			bool has_value_flag;
		};
	public:
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
			return MOptional<T>(NoneOpt());
		}
	private:
		MOptionalBase m_base;

	public:
		static NullOpt None;

	public:
		MOptional() {}
		explicit MOptional(NoneOpt) {}
		template<typename... Args>
		MOptional(InPlacement, Args ... args) {}
	public:
		T unwrap() {
			assert(this->m_base->has_value_flag == false);
			constexpr if (!std::is_reference<T>::value) {
				return static_cast<T>(this->m_base.data);
			}
			else {
				return static_cast<T>(this->m_base.data).get();
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
