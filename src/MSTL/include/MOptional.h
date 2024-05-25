#ifndef __MUZI_MOPTIONAL_H__
#define __MUZI_MOPTIONAL_H__
#include<optional>
#include<assert.h>
namespace MUZI {
	template<typename T>
	class MOptional {
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
				T value;
			}data;
			bool has_value_flag;
		};

	private:
		MOptionalBase m_base;
	public:
		struct NullOpt { // no-value state indicator
			struct _Tag {};
			constexpr explicit NullOpt(_Tag) {}
		};
		struct InPlacement {
			explicit InPlacement() = default;
		};
	public:
		static NullOpt None;
	public:
		MOptional() {}
		explicit MOptional(NullOpt) {}
		template<typename... Args>
		MOptional(InPlacement, Args ... args) {
		}
	public:
		T unwrap() {
			assert(this->m_base->has_value_flag == false);
			return static_cast<T>(this->m_base.data);
		}

		bool is_Null() {
			return !this->m_base.has_value_flag;
		}
	public:
		void reset() {
			this->m_base.has_value_flag = false;
		}
	public:
		bool operator==(NullOpt None) {
			return !this->m_base.has_value_flag;
		}
	};
}

#endif // !__MUZI_MOPTIONAL_H__
