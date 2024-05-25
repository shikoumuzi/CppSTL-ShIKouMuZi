#ifndef __MUZI_MRESULT_H__
#define __MUZI_MRESULT_H__

template<typename T, typename Err>
class MResult {
private:
	T value;
	bool is_error;
	Err error;
public:
	MResult(T value) {
		
	}
	MResult(T value, Err error) {

	}
public:
	T unwrap() {
		return value;
	}
	inline bool is_err() {
		return this->is_error;
	}
};



#endif // !__MUZI_MRESULT_H__
