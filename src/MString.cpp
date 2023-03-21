#include"MString.h"



// MStringData 负责控制数据处理和内存申请
class MStringData
{
private:

public:
	MStringData(const char32_t* cstr = nullptr)
	{

	}
	MStringData(const MStringData& str)
	{

	}	
	MStringData(MStringData&& str)
	{

	}
	~MStringData()
	{

	}
public:

private:// 浅拷贝和深拷贝函数
	void shallow_copy()
	{

	}
	void deep_copy()
	{

	}
private:
	char32_t* data;// utf-32
};






// MString 负责各种编码转换
MString::MString()
{
}

MString::~MString()
{
}