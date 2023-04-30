<<<<<<< HEAD
#include<string.h>
=======
>>>>>>> 5270042c6f209941ee8c2fd9fce36cbb1eb7f638
#include"MAllocator.h"
#include"MString.h"

namespace MUZI
{

	// MStringData 负责控制数据处理和内存申请
	class MStringData
	{
	public:
		friend class MString;
	private:
		struct DataPackeg
		{
			char32_t* data;// utf-32
			size_t str_len;
			size_t str_real_len;// 包括背后\0
		};
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
			this->allocate->deallocate(this->m_data.data, this->m_data.str_real_len);
			
		}
	public:
		MStringData& Read()
		{

		}
		int Write(MStringData& other)
		{

		}
	public:
		// 设定内存管理方式 
		bool setAllocation(MAllocator* allocator)
		{
			if (this->allocate != nullptr)
			{
				return false;
			}
			this->allocate = allocator;
			return true;
		}
	private:
		inline size_t get_real_size()
		{
			return this->m_data.str_len + 1;
		}

	private:// 浅拷贝和深拷贝函数
		void shallow_copy(DataPackeg& o_data)
		{
			if(this->m_data.data != nullptr)
				this->allocate->deallocate(this->m_data.data, this->m_data.str_real_len);
			memcpy(&this->m_data, &o_data, sizeof(struct DataPackeg));
		}
		void deep_copy(DataPackeg& o_data)
		{
			shallow_copy(o_data);
			this->m_data.data = static_cast<char32_t*>(this->allocate->allocate(this->m_data.str_real_len));
			memcpy(this->m_data.data, o_data.data, this->m_data.str_real_len);
		}
	private:
		MAllocator* allocate;
		DataPackeg m_data;
	};






	// MString 负责各种编码转换 和基本数据获取
	MString::MString():p_data(nullptr)
	{}

	MString::MString(const MString& other)
	{
		if (&other == this)
		{
			return;
		}
	}

	MString::MString(MString&& other)
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