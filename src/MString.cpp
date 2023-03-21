#include"MAllocator.h"
#include"MString.h"

namespace MUZI
{

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
		MAllocator* allocate;
	};






	// MString ������ֱ���ת��
	MString::MString():p_data(nullptr)
	{}

	MString::MString(const MString& other)
	{
		if (&other == this)
		{
			return;
		}
	}

	MString::MString(MString&& other)noexcept
	{
		if (&other == this)
		{
			return;
		}
		MStringData* tmp = this->p_data;
		this->p_data = other.p_data;
		other.p_data = nullptr;
	}

	MString::~MString()
	{
		if (nullptr != p_data)
		{
			delete this->p_data;
		}
	}

}