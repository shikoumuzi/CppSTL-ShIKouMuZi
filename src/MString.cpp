#include"MString.h"



// MStringData ����������ݴ�����ڴ�����
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

private:// ǳ�������������
	void shallow_copy()
	{

	}
	void deep_copy()
	{

	}
private:
	char32_t* data;// utf-32
};






// MString ������ֱ���ת��
MString::MString()
{
}

MString::~MString()
{
}