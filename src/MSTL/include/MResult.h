#ifndef __MUZI_MRESULT_H__
#define __MUZI_MRESULT_H__
#include<iostream>
#include<assert.h>
#include "MOptional.h"

namespace MUZI {
	template<typename T, typename Err, bool = std::enable_if<!std::is_reference<T>::value, T>::value>
	class MResultBase {
	public:
		using value_type = std::enable_if<!std::is_reference<T>::value, T>::type;
	protected:
		value_type value;
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
				return this->value;
			}
		}
	public:
	};

	template<typename T, typename Err>
	class MResultBase<T, Err, false> {
	public:
		using value_type = std::enable_if<std::is_lvalue_reference<T>::value, T>::type;
		using value_without_ref_type = std::remove_reference<value_type>::type;
	protected:
		value_type value;
		bool is_error_flag;
		Err error;
	public:
		MResultBase(T value) : value(value) {
		}
		MResultBase(Err error) : error(value) {
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
		MResult(T value) :MResultBase<T, Err>(value) {}
		MResult(Err err) :MResultBase<T, Err>(err) {}
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
				return MOptional<T>(MOptional<T>::InPlacement(), value);
			}
		}
		MOptional<Err> error() {
			if (this->is_error_flag) {
				return MOptional<T>(MOptional<T>::InPlacement(), Err());
			}
			else {
				return MOptional<T>::None;
			}
		}
	};
}

#endif // !__MUZI_MRESULT_H__
