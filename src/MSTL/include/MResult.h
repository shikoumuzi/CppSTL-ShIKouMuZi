#ifndef __MUZI_MRESULT_H__
#define __MUZI_MRESULT_H__
#include<iostream>
#include<assert.h>
#include "MOptional.h"

namespace MUZI {
	template<typename T, typename Err, bool = std::enable_if<!std::is_reference<T>::value, T>::value>
	class MResultBase {
	public:
		using value_type = std::enable_if<std::is_reference<T>::value, T>::type;
	protected:
		T value;
		Err error;
		bool is_error_flag;

	public:
		MResultBase(T value) :
			value(value),
			is_error_flag(false) {}
		MResultBase(Err error) :
			error(error),
			is_error_flag(true) {}
	public:

		value_type unwrap_or_default() {
			if (this->is_error_flag) {
				return value_type();
			}
			else {
				return value;
			}
		}
	public:
	};

	template<typename T, typename Err>
	class MResultBase<T, Err, false> {
	public:
		using value_type = std::enable_if<!std::is_reference<T>::value, T>::type;
		using value_without_ref_type = std::remove_reference<value_type>::type;
	protected:
		T value;
		bool is_error_flag;
		Err error;
	private:
		static value_without_ref_type default_value;
	public:
		MResultBase(T value) {
		}
		MResultBase(T value, Err error) {
		}
	public:
		MOptional<T> unwrap_or_default() {
			if (this->is_error_flag) {
				return MOptional<T>::None;
			}
			else {
				return MOptional<T>(MOptional<T>::InPlacement(), value);
			}
		}

	public:
	};

	template<typename T, typename Err>
	class MResult : public MResultBase<T, Err> {
	public:
		MResultBase<T, Err>::value_type unwrap() {
			assert(this->is_error_flag == true);
			return value;
		}
	public:
		inline bool is_err() {
			return this->is_error_flag;
		}
		inline bool is_ok() {
			return !this->is_error_flag;
		}
	public:
		MOptional<T> ok() {
			if (this->is_error_flag) {
				return MOptional<T>::None;
			}
			else {
			}
		}
		MOptional<Err> error() {
			if (this->is_error_flag) {
			}
			else {
				return MOptional<T>::None;
			}
		}
	};
}

#endif // !__MUZI_MRESULT_H__
