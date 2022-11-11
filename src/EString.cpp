#define _CRT_NON_CONFORMING_SWPRINTFS
#include"EString.h"
#include<map>
using namespace std;

//class EStringprivate*str的形式
//用宏隐藏，
//1、方便在编译成dll后，随时更换
//2、使得用户无法得知内部数据结构
//3、用户版本升级时不用更换头文件
E_PRIVATE(EString)
{
public:
	wchar_t* Edata;
	size_t len;
	size_t real_size;
	EString* that;

public:
	//size_t s_int()
	//{

	//}
	//int i_size_t()
	//{

	//}
	void data_realloc(size_t size_after,bool len = true)
	{
		if (this->real_size < size_after)
		{
			(len == false) ? 0 : this->len = size_after;

			size_after += 1;
			this->real_size = size_after + 1;
			wchar_t* p = this->Edata;
			if ((this->Edata = (wchar_t*)realloc(this->Edata, size_after * sizeof(wchar_t))) != NULL);
			else if ((p = (wchar_t*)malloc(sizeof(wchar_t) * size_after)) != NULL)
			{
				memcpy(p, this->Edata, this->len);
				free(this->Edata);
				this->Edata = p;
			}
			else
			{
				throw EString("can not realloc or malloc");
			}
			this->Edata[size_after] = L'\0';
		}
		else
		{
			(len == false) ? 0 : this->len = size_after;
		}
	}
	wchar_t* string_cut(size_t begin = 0, size_t end = 0)
	{
		if (end < begin)
		{
			throw EString("end cannot smaller than begin");
		}
		if (end == 0 || end>this->len)
		{
			end = this->len;
		}
		wchar_t* rt = (wchar_t*)malloc((end - begin)*sizeof(wchar_t));
		wchar_t* p = rt;
		for (size_t i = begin; i < end; ++i)
		{
			rt[i - begin] = this->Edata[i];
		}
		return rt;
	}
};

//迭代器
E_PRIVATE(Iterator)
{
public:
	EString* parent;//存储外部类指针，方便内部类操作
	wchar_t* iter;
	bool is_invalid;
public:
	bool isvalid()
	{
		if (!(this->is_invalid))
		{
			throw "Iterator is invalid";
			return false;
		}
		else if (parent == nullptr)
		{
			return false;
		}
		else if (iter == nullptr)
		{
			return false;
		}
		return true;
	}
};
//构造函数
EString::Iterator::Iterator()
	:it(new E_PRIVATE(Iterator))
{
	this->it->parent = nullptr;//通过偏移量计算出外部类的指针
	this->it->iter = nullptr;
	this->it->is_invalid = false;
}
EString::Iterator::Iterator(EString& base)
	:it(new E_PRIVATE(Iterator))
{
	this->it->parent = &base;//获取父类地址
	/*this->parent = (EString*)((char*)this - offsetof(EString, str_iter));*/
	this->it->iter = base.d->Edata;
	this->it->is_invalid = false;

}
EString::Iterator::Iterator(EString::Iterator& rhs)
	:it(new E_PRIVATE(Iterator))
{
	if ( rhs.it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		this->it->parent = rhs.it->parent;//获取父类地址
		this->it->iter = rhs.it->iter;
		this->it->is_invalid = false;
	}
}
EString::Iterator::Iterator(EString::Iterator&& rhs)
{
	if (rhs.it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		this->it = rhs.it;
		rhs.it = new E_PRIVATE(Iterator);
		rhs.it->parent = nullptr;
		rhs.it->iter = nullptr;
	}
}
EString::Iterator::Iterator(const EString::Iterator& rhs)
	:it(new E_PRIVATE(Iterator))
{
	this->it->parent = rhs.it->parent;//获取父类地址
	this->it->iter = rhs.it->iter;
	this->it->is_invalid = false;

}

//重载运算符
wchar_t EString::Iterator::operator* ()
{
	if (this->it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		if (*(this->it->iter) == '\0' && (this->it->iter + 1) == 0)
		{
			throw EString("out of range");
		}
		return *(this->it->iter);
	}
}
bool EString::Iterator::operator==(EString::Iterator& rhs)
{
	if (this->it->is_invalid == true || rhs.it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		return this->it->iter == rhs.it->iter;
	}
}
bool EString::Iterator::operator!=(EString::Iterator& rhs)
{
	return !(*this == rhs);
}
bool EString::Iterator::operator<(EString::Iterator& rhs)
{
	if (this->it->is_invalid == true || rhs.it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		if (this->it->iter >= rhs.it->iter)
		{
			return false;
		}
		return true;
	}
}
bool EString::Iterator::operator>(EString::Iterator& rhs)
{
	if (this->it->is_invalid == true || rhs.it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		if (this->it->iter <= rhs.it->iter)
		{
			return false;
		}
		return true;
	}
}
bool EString::Iterator::operator>=(EString::Iterator& rhs)
{
	return !((*this) < rhs);
}
bool EString::Iterator::operator<=(EString::Iterator& rhs)
{
	return !((*this) > rhs);
}
EString::Iterator& EString::Iterator::operator++()
{
	if (this->it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		if (this->it->iter == nullptr)
		{
			throw "Iterator is invaild";
		}
		else if (*(this->it->iter) == '\0' && (this->it->iter + 1) == 0)
		{
			throw EString("out of range");
		}
		++(this->it->iter);
		return *this;
	}
}
const EString::Iterator EString::Iterator::operator++(int)
{
	if (this->it->is_invalid == true )
	{
		throw EString("iterator is invalid");
	}
	else
	{
		if (this->it->iter == nullptr)
		{
			throw "Iterator is invaild";
		}
		else if (*(this->it->iter) == '\0' && (this->it->iter + 1) == 0)
		{
			throw EString("out of range");
		}
		EString::Iterator temp(*this);//保存本身值
		++(this->it->iter);
		return temp;
	}
}
EString::Iterator& EString::Iterator::operator--()
{
	if (this->it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		if (this->it->iter == nullptr)
		{
			throw "Iterator is invaild";
		}
		else if (this->it->parent->d->Edata >= this->it->iter)
		{
			throw EString("out of range");
		}
		--(this->it->iter);
		return *this;
	}
}
const EString::Iterator EString::Iterator::operator--(int)
{
	if (this->it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		if (this->it->iter == nullptr)
		{
			throw "Iterator is invaild";
		}
		else if (this->it->parent->d->Edata >= this->it->iter)
		{
			throw EString("out of range");
		}
		EString::Iterator temp(*this);//保存本身值
		--(this->it->iter);
		return temp;
	}
}
EString::Iterator& EString::Iterator::operator=(const EString::Iterator& rhs)
{
	if (this->it->is_invalid == true || rhs.it->is_invalid == true)
	{
		throw EString("iterator is invalid");
	}
	else
	{
		this->it->parent = rhs.it->parent;//获取父类地址
		this->it->iter = rhs.it->iter;
		return *this;
	}
}

//string部分
EString::EString()
	:d(new E_PRIVATE(EString))
{
	d->Edata = nullptr;
	d->len = 0;
	d->real_size = 0;
}
EString::EString(EString& other)
	:d(new E_PRIVATE(EString))
{
	this->d->data_realloc(other.d->len);
	wchar_t* p = this->d->Edata, * q = other.d->Edata;
	while (*p++ = *q++);
}
EString::EString(EString&& other) noexcept
	:d(other.d)
{
	other.d = new E_PRIVATE(EString);
	other.d->Edata = nullptr;
	other.d->len = 0;
	other.d->real_size = 0;
}
EString::EString(const EString& other)
	:d(new E_PRIVATE(EString))
{
	this->d->data_realloc(other.d->len);
	wchar_t* p = this->d->Edata, * q = other.d->Edata;
	while (*p++ = *q++);//memcpy(this->d->Edata, other.d->Edata,other.d->len )
}
EString::EString(const char*  other)
	:d(new E_PRIVATE(EString))
{
	int len_for_other = strlen(other);
	this->d->data_realloc(len_for_other);
	//c stdlib.h所提供的宽窄字符串转换
	if (mbstowcs(this->d->Edata, other, len_for_other) != -1);
	else
	{
		throw EString("char to wchar_t was failing");
	}
}
EString::EString(const wchar_t* other)
	:d(new E_PRIVATE(EString))
{
	int len_for_other = wcslen(other);
	this->d->data_realloc(len_for_other);
	//c stdlib.h所提供的宽窄字符串转换
	memcpy(this->d->Edata, other, len_for_other);
}
EString::EString(wchar_t&& other)
	:d(new E_PRIVATE(EString))
{
	this->d->Edata = &other;
	this->d->len = wcslen(this->d->Edata) + 1;
	other = L'\0';
}
EString::EString(char* other)
	:d(new E_PRIVATE(EString))
{
	int len_for_other = strlen(other);
	this->d->data_realloc(len_for_other);
	//c所提供的宽窄字符串转换
	mbstowcs(this->d->Edata, other, len_for_other + 2) != -1;
}
EString::EString(char other)
	:d(new E_PRIVATE(EString))
{
	this->d->data_realloc(1);
	//c所提供的宽窄字符串转换
	mbstowcs(this->d->Edata, &other, 1);
}
EString::EString(EString::Iterator& begin, EString::Iterator& end)
{
	this->d->len = (end.it->iter - begin.it->iter) / sizeof(wchar_t);
	this->d->data_realloc(this->d->len);
	wchar_t* p = this->d->Edata, * q = begin.it->iter;
	for (; p != end.it->iter; ++p)
	{
		*p = *q++;
	}
}
EString::EString(EString& str, size_t begin, size_t end)
{
	if (str.d->Edata == 0)
	{
		throw EString("str is null");
	}
	if(end > str.d->len)
	{
		end = str.d->len;
	}
	if (end > begin)
	{
		wchar_t* p = str.d->Edata + begin, * q = str.d->Edata + end;
		size_t len = (q - p) / sizeof(wchar_t);
		this->d->data_realloc(len);

		wchar_t* o = this->d->Edata;
		for (; p != q; p++, o++)
		{
			*o = *p;
		}
	}
	else
	{
		throw EString("end can not less than begin");
	}
}
EString::~EString()
{
	free(this->d->Edata);
	this->d->Edata = nullptr;
	delete d;
	d = nullptr;
}

//格式化输出
EString	EString::arg(const EString& a /* = QLatin1Char(' ') */) const
{
	char buffer[10] = { 0 };
	EString rt;

	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = d->Edata;

	wchar_t symbol[3] = { L"%" };

	for (int i = 0; i < sizeof(index); ++i)
	{
		//symbol[1] = A2W(itoa(index[i], buffer, 10))[0];
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += a;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	
	return rt;
}
EString	EString::arg(long long a, /*int fieldWidth,*/ int base/*, char fillChar /* = QLatin1Char(' ') */) const
{
	EString value = to_string((long long)a,base);
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		//symbol[1] = A2W(itoa(index[i], buffer, 10))[0];
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += value;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
EString	EString::arg(unsigned long long a,/* int fieldWidth, */int base/*, char fillChar  /* = QLatin1Char(' ') */) const
{
	EString value = to_string((unsigned long long)a, base);
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		//symbol[1] = A2W(itoa(index[i], buffer, 10))[0];
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += value;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
EString	EString::arg(long a,/*, int fieldWidth,*/ int base/*, char fillChar  /* = QLatin1Char(' ') */) const
{
	EString value = to_string((long)a, base);
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		//symbol[1] = A2W(itoa(index[i], buffer, 10))[0];
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += value;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
//EString	EString::arg(long a, int fieldWidth , int base , char fillChar  /* = QLatin1Char(' ') */) const
//{
//	EString out(*this);
//	return out;
//}
EString	EString::arg(int a,/* int fieldWidth, */int base/*, char fillChar /* = QLatin1Char(' ') */) const
{
	EString value = to_string((int)a, base);
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += value;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
//EString	EString::arg(int a, int fieldWidth , int base , char fillChar /* = QLatin1Char(' ') */) const
//{
//	EString out(*this);
//	return out;
//}
EString	EString::arg(short a,/* int fieldWidth,*/ int base/*, char fillChar /* = QLatin1Char(' ') */) const
{
	EString value = to_string((short)a, base);
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += value;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
EString	EString::arg(unsigned short a, /*int fieldWidth,*/ int base/*char fillChar /* = QLatin1Char(' ') */) const
{
	EString value = to_string((unsigned short)a, base);
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += value;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
EString	EString::arg(double a/* int fieldWidth, char format, int precision, char fillChar /* = QLatin1Char(' ') */) const
{
	EString value = to_string((double)a,10);
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += value;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
EString	EString::arg(char a/*, int fieldWidth, char fillChar /* = QLatin1Char(' ') */) const
{
	EString rt;
	char buffer[10] = { 0 };
	constexpr int index[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	wchar_t* pos = this->d->Edata;
	wchar_t symbol[3] = { L"%" };
	for (int i = 0; i < sizeof(index); ++i)
	{
		buffer[i] = index[i] + 0x30;
		mbstowcs(&symbol[1], &buffer[i], 1);
		symbol[2] = L'\0';

		pos = wcsstr(d->Edata, symbol);
		if (pos)
		{
			do
			{
				int leftStr_len = (euint64(pos) - euint64(d->Edata)) / sizeof(wchar_t);//找到symbol[i]
				wchar_t* leftstr = (wchar_t*)alloca((leftStr_len + 1) * sizeof(wchar_t));
				memcpy(leftstr, d->Edata, leftStr_len * sizeof(wchar_t));//复制前半段
				leftstr[leftStr_len] = L'\0';

				rt += leftstr;
				rt += a;
				rt += pos + wcslen(symbol);
				pos = wcsstr(d->Edata, symbol);
			} while (pos);
			break;
		}
		else break;
	}
	return rt;
}
//EString	EString::arg(char a, int fieldWidth , char fillChar /* = QLatin1Char(' ') */) const
//{
//	EString out(*this);
//	return out;
//}
//push
template<typename...Args>
EString	EString::arg(Args&&... args) const
{
	size_t count = sizeof...(args);
	if (count > 9 || count == 0)
	{
		throw EString("It have more or less args");
	}
	else
	{
		int temp[] = { (this->arg(args),0)... };
	}
}

void EString::push_back(const EString& other)
{
	this->d->data_realloc(this->d->len + other.d->len);
	wchar_t* p = this->d->Edata + this->d->len-(other.d->len), * q = other.d->Edata;
	while (*p++ = *q++);
}
void EString::push_back(const char& other)
{
	this->d->data_realloc(1 + this->d->len);
	this->d->Edata[this->d->len] = other;
}
void EString::push_back(char* const other)
{
	int other_len = strlen(other) ;
	this->d->data_realloc(other_len + this->d->len);
	char* p = other;
	wchar_t* q = this->d->Edata + this->d->len - other_len;
	while (*q++ = *p++);
}
void EString::push_back(wchar_t* other)
{
	if (other != nullptr)
	{
		int other_len = wcslen(other) + 1;
		this->d->data_realloc(other_len);
		wchar_t* p = other;
		wchar_t* q = this->d->Edata + this->d->len - other_len;
		while (*q++ = *p++);
	}
	else
	{
		throw EString("other is invalid");
	}
}
void EString::push_back(wchar_t other)
{
	int other_len = wcslen(&other);
	this->d->data_realloc(this->d->len + other_len);
	wchar_t* q = this->d->Edata + this->d->len - other_len;
	memcpy(q, &other, other_len);
}
void EString::push_front(const EString& other)
{
	this->d->data_realloc(this->d->len + other.d->len);
	for (int i = this->d->len + other.d->len - 1; i >= this->d->len; --i)
	{
		this->d->Edata[i] = this->d->Edata[i - other.d->len];//移动代码
	}
	wchar_t* p = this->d->Edata, * q = other.d->Edata;
	while (*p++ = *q++);
}
void EString::push_front(char* const other)
{
	int other_len = strlen(other);
	this->d->data_realloc(other_len + this->d->len);
	for (int i = this->d->len + other_len - 1; i >= this->d->len; --i)
	{
		this->d->Edata[i] = this->d->Edata[i - other_len];
	}
	this->d->len += other_len;
	char* p = other;
	wchar_t* q = this->d->Edata;
	while (*q++ = *p++);
}

//属性调用
int	EString::compare(const EString& other) const
{
	return wcscmp(this->d->Edata, other.d->Edata);
}
int	EString::compare(const char* other) const
{
	size_t len = strlen(other);
	wchar_t* p = (wchar_t*)malloc(sizeof(wchar_t) * (len));
	mbstowcs(p, other, len);
	int compare_value = wcscmp(this->d->Edata, p);
	free(p);
	return compare_value;
}
int	EString::compare(const wchar_t* other) const
{
	return wcscmp(this->d->Edata, other);
}
wchar_t EString::back() const
{
	if (this->d->Edata!=nullptr)
	{
		return this->d->Edata[this->d->len - 1];
	}
	else
	{
		throw EString("Estring is invalid");
	}
}
wchar_t& EString::back()
{
	if (this->d->Edata != nullptr)
	{
		return this->d->Edata[this->d->len - 1];
	}
	else
	{
		throw EString("Estring is invalid");
	}
}
void EString::clear()
{
	free(this->d->Edata);
	this->d->Edata = nullptr;
	this->d->len = 0;
}
size_t EString::size() const
{
	return this->d->len;
}
char* EString::c_str() const
{
	char* result=nullptr;
	if (wcstombs(result, this->d->Edata, this->d->len + 2) != -1)
	{
		return result;
	}
	else
	{
		throw EString("wchar_t to char was failing");
		return nullptr;
	}
}
wchar_t* EString::data() const
{
	return this->d->Edata;
}

//增删查替排
EString::Iterator EString::begin()
{
	EString::Iterator it(*this);
	return it;
}
EString::Iterator EString::end()
{
	EString::Iterator it(*this);
	it.it->iter += this->d->len + 1;
	return it;
}
EString EString::at(size_t index)
{
	return this->operator[](index);
}
EString& EString::append(const EString& str)
{
	(*this) += str;
	return *this;
}
EString& EString::append(char ch)
{
	this->push_back(ch);
	return *this;
}
EString& EString::append(const char* str, size_t len)
{
	wchar_t* p = (wchar_t*)malloc(sizeof(wchar_t) * len);
	mbstowcs(p, str, len);
	this->d->Edata = (wchar_t*)realloc(this->d->Edata,sizeof(wchar_t) * len + this->d->len + 2);
	for (int i = 0; i < len ; ++i)
	{
		this->d->Edata[this->d->len + i] = str[i];
	}
	int len_this = len + this->d->len;
	this->d->Edata[len_this - 3] = '\0';
	this->d->Edata[len_this - 2] = '\0';
	free(p);
	return *this;
}
EString& EString::append(const char* str)
{
	this->push_back(str);
	return *this;
}
EString::Iterator EString::earse(EString::Iterator& begin, EString::Iterator& end)
{
	if (begin.it->is_invalid || end.it->is_invalid)
	{
		throw "cannot use invalid iterator";
	}
	wchar_t* p = begin.it->iter;
	wchar_t* q = end.it->iter;
	wchar_t* temp = p;
	int count = 0;
	while (temp++ != q)
		count++;

	while (*p++ = *q++);
	begin.it->parent->d->Edata = (wchar_t*)realloc(begin.it->parent->d->Edata, begin.it->parent->d->len - count + 2);


	begin.it->is_invalid = true;
	end.it->is_invalid = true;
	
	EString::Iterator it(*this);
	it.it->iter = begin.it->iter + 1;
	return it;
}
EString::Iterator EString::earse(EString::Iterator& begin)
{
	EString::Iterator it = this->end();
	return this->earse(begin, it);
}
EString& EString::insert(EString::Iterator& begin, EString& other)
{
	return this->insert(begin, (wchar_t*)(other.d->Edata));
}
EString& EString::insert(EString::Iterator& begin, EString&& other)
{
	return this->insert(begin, (wchar_t*)(other.d->Edata));
}
EString& EString::insert(EString::Iterator& begin, const char* other)
{
	size_t len_other = strlen(other);
	wchar_t* p = (wchar_t*)malloc(len_other * sizeof(wchar_t));
	mbstowcs(p, other, len_other);
	this->insert(begin, p);
	free(p);
	return *this;
}
EString& EString::insert(EString::Iterator& begin, char* other)
{
	return this->insert(begin,(const char*)other);
}
EString& EString::insert(EString::Iterator& begin, char other)
{
	return this->insert(begin, (const char*)(&other));
}
EString& EString::insert(EString::Iterator& begin, wchar_t* other)
{
	if (this == begin.it->parent)
	{
		if (begin.it->isvalid())
		{
			size_t len_other = wcslen(other);
			size_t len_self = wcslen(begin.it->iter);
			size_t len = begin.it->parent->d->len + len_other;
			this->d->data_realloc(len);
			wchar_t* p = begin.it->iter, * q = other;
			for (int i = 0; i < len_self; ++i)
			{
				p[len_other + i] = p[i];
			}
			for (int i = 0; i < len_other; ++i)
			{
				p[i] = q[i];
			}
			this->d->len = len;
			begin.it->is_invalid = true;
			return *this;
		}
		else
		{
			throw EString("iteratot is invalid");
		}
	}
	else
	{
		throw EString("Iterator is not belong to EString");
	}
}
EString& EString::insert(EString::Iterator& begin, wchar_t other)
{
	return this->insert(begin, (wchar_t*)(&other));
}
EString& EString::fill(char ch, size_t size)
{
	size_t relen = 0;
	(size <= -1) ? relen = this->d->len : relen = size;
	this->d->data_realloc(size);
	for (size_t i = 0; i < relen; ++i)
	{
		mbstowcs(&(this->d->Edata[i]), &ch, 1);
	}
	return *this;
}
EString EString::first(size_t size)
{
	if (size < 0)
	{
		throw EString("out of range");
	}
	else
	{
		EString temp;
		temp.d->data_realloc(size);
		for (size_t i = 0; i < size; ++i)
		{
			temp[i] = (*this)[i];
		}
	}

}
size_t EString::find(EString& str, size_t index, size_t end )
{
	return this->find((wchar_t*)str.d->Edata,index);
}
size_t EString::find(const EString& str, size_t index, size_t end)
{
	return this->find((wchar_t*)str.d->Edata,index);
}
size_t EString::find(char* str, size_t index, size_t end)
{
	size_t len = strlen(str);
	wchar_t* p = (wchar_t*)malloc(sizeof(wchar_t) * len);
	mbstowcs(p, str, len);
	size_t result = this->find((const wchar_t*)p,index);
	free(p);
	return result;
}
size_t EString::find(const char* str, size_t index, size_t end)
{
	size_t len = strlen(str);
	wchar_t* p = (wchar_t*)malloc(sizeof(wchar_t) * len);
	mbstowcs(p, str, len);
	size_t result = this->find((const wchar_t*)p,index);
	free(p);
	return result;
}
size_t EString::find(wchar_t* str, size_t index, size_t end)
{
	return this->find((wchar_t*)str);
}
size_t EString::find(const wchar_t* str, size_t index, size_t end)
{
	size_t len_str = wcslen(str);
	size_t len_self = this->d->len;
	if (end == 0)
	{
		end = len_self;
	}
	if (len_str > len_self || len_str <= 0 || str == nullptr || end < index)
	{
		return this->npos;
	}
	else
	{
		if(len_str < 1000)
		{

		}
		else //Boyer - Moore
		{
			map<wchar_t, size_t> bad_map;
			int* GS = (int*)alloca(sizeof(int) * len_str);
			size_t bad = 0, good = 0;
			//坏字符生成
			for (size_t i = 0; i < len_str - 1; ++i)
			{
				bad_map[str[i]] = i + 1;
			}

			for (size_t i = len_str; i < len_self;)
			{
				for (size_t j = i; j > i-len_str; --j)
				{
					if (this->d->Edata[j] != str[i - j])
					{
						i += (bad_map[str[i - j]] > GS[i - j]) ? bad_map[str[i - j]] : GS[i - j];
						break;
					}
				}
			}
		}
	}
}
bool EString::startsWith(const EString& s) const
{
	return this->startsWith(s.d->Edata);
}
bool EString::startsWith(char* c) const
{
	size_t len = strlen(c);
	wchar_t* p = (wchar_t*)malloc(sizeof(wchar_t) * len);
	mbstowcs(p, c, len);
	bool result = this->startsWith((wchar_t*)p);
	free(p);
	return result;
}
bool EString::startsWith(wchar_t* c) const
{
	size_t len_c = wcslen(c);
	size_t len_self = this->d->len;
	if (len_c < len_self)
	{
		return false;
	}
	else
	{
		wchar_t* p = this->d->Edata, * q = c;
		for (size_t i = 0; i <= len_c; ++i)
		{
			if (*p != *q)
			{
				return false;
			}
		}
		return true;
	}
}
bool EString::endsWith(const EString& s) const
{
	return this->endsWith(s.d->Edata);
}
bool EString::endsWith(char* c) const
{
	size_t len_c = strlen(c);
	wchar_t* p = (wchar_t*)malloc(sizeof(wchar_t) * len_c);
	mbstowcs(p, c, len_c);
	bool temp = this->endsWith((wchar_t*)p);
	free(p);
	return temp;
}
bool EString::endsWith(wchar_t* c) const
{
	size_t len_c = wcslen(c);
	size_t len_self = this->d->len;
	if (len_c > len_self)
	{
		return false;
	}
	else
	{
		wchar_t* p = this->d->Edata + len_self;
		wchar_t* q = c + len_c;
		for (size_t i = len_c - 1; i >= 0; --i, --p, --q)
		{
			if (*p != *q)
			{
				return false;
			}
		}
		return true;
	}
}
EString& EString::replace(size_t position, size_t n, const EString& after)
{
	size_t len_after = after.d->len;
	size_t len_self = this->d->len;
	wchar_t* p = this->d->Edata;
	if (position >= len_after)
	{
		return *this;
	}
	
	if (len_after > n)
	{
		this->d->data_realloc(len_after + len_self);
	}
	else if (len_after < n)
	{
		for (size_t i = position + len_after ; i < len_self; ++i)
		{
			p[i] = p[i + 1];
		}
		this->d->data_realloc(len_after + len_self);
		for (size_t i = position; i < position + n; ++i)
		{
			p[i] = after.d->Edata[i - position];
		}
	}
	else
	{
		for (size_t i = position; i < position + n; ++i)
		{
			p[i] = after.d->Edata[i - position];
		}
	}

	return *this;
}
EString& EString::replace(size_t position, size_t n, char after)
{
	return this->replace(position,n,(const char *)&after);
}
EString& EString::replace(size_t position, size_t n, const wchar_t* after)
{
	EString temp(after);
	return this->replace(position, n,temp);
}
EString& EString::replace(size_t position, size_t n, const char* unicode, size_t size)
{
	EString str(unicode);
	str.d->len = size;
	return this->replace(position, n, str);
}
EString& EString::replace(char before, char after)
{
	wchar_t before_w = L'\0', after_w = L'\0';
	mbstowcs(&before_w, &before, 1);
	mbstowcs(&after_w, &after, 1);

	for (wchar_t* p = this->d->Edata; p != NULL; ++p)
	{
		if (*p == before_w)
		{
			*p = after_w;
		}
	}
	return *this;
}
EString& EString::replace(const char* before, size_t blen, const char* after, size_t alen)
{
	EString before_E(before);
	before_E.d->len = blen;
	EString after_E(after);
	after_E.d->len = alen;
	return this->replace(before_E, after_E);
}
EString& EString::replace(const EString& before, const EString& after)
{
	for (size_t i = 0; i < this->d->len; ++i)
	{
		if ((i = this->find(before, i)) != this->npos)
		{
			this->replace(i, before.d->len, after);
		}
		else
		{
			break;
		}
	}
	return *this;
}
EString& EString::replace(char ch, const EString& after)
{
	wchar_t c_w = L'\0';
	mbstowcs(&c_w, &ch, 1);
	for (size_t i = 0; i < this->d->len; ++i)
	{
		if (this->d->Edata[i] == c_w)
		{
			this->replace(i, 1, after);
		}
	}
	return *this;
}
EString	EString::repeated(size_t times) const
{
	EString rt(*this);
	for (size_t i = 1; i < times; ++i)
	{
		rt += *this;
	}
	return rt;
}
void EString::reserve(size_t size)
{
	size_t len = this->d->len;
	this->d->data_realloc(size,false);
	this->d->len = len;
}
void EString::resize(size_t size)
{
	size_t len_self = this->d->len;
	size_t len_real = this->d->real_size;
	this->d->data_realloc(size);
	if (size > len_real)
	{
		this->d->len = len_self;
	}
}
void EString::resize(size_t size, char fillChar)
{
	size_t len_self = this->d->len;
	wchar_t fillcahr_w = L'\0';
	mbstowcs(&fillcahr_w, &fillChar, 1);
	this->d->data_realloc(size);
	if(size > len_self)
	{
		this->d->data_realloc(size);
		for (size_t i = len_self;i<this->d->len; ++i)
		{
			this->d->Edata[i] = fillcahr_w;
		}
	}
}
EString	EString::right(size_t n) const
{
	if (n > this->d->len)
	{
		return *this;
	}
	else
	{
		wchar_t* rt_w = (wchar_t*)malloc(sizeof(wchar_t) * n);
		for (size_t i = 0; i < n; ++i)
		{
			rt_w[i] = this->d->Edata[i];
		}
		return EString(std::move(rt_w));
	}
}
EString& EString::remove(size_t position, size_t n)
{
	for (size_t i = position; i < position + n; ++i)
	{
		this->d->Edata[i] = this->d->Edata[i + n];
	}
	this->d->data_realloc(this->d->len - n);
	return *this;
}
EString& EString::remove(EString::Iterator& begin, EString::Iterator& end)
{
	if (begin.it->isvalid() && end.it->isvalid())
	{
		size_t pos = (begin.it->iter - this->d->Edata) / sizeof(wchar_t);
		size_t n = (begin.it->iter - end.it->iter) / sizeof(wchar_t);
		return this->remove(pos, n);
	}
	else
	{
		throw EString("Iterator is invalid");
	}
}
EString& EString::remove(char ch)
{
	return this->remove(&ch);
}
EString& EString::remove(char* str)
{
	if (size_t pos = this->find(str) != this->npos)
	{
		size_t n = strlen(str);
		return this->remove(pos, n);
	}
	else
	{
		throw EString("can not find str");
	}
}
EString& EString::remove(wchar_t* str)
{
	if (size_t pos = this->find(str) != this->npos) 
	{
		size_t n = wcslen(str);
		return this->remove(pos, n);
	}
	else
	{
		throw EString("can not find str");
	}
}
EString& EString::remove(const EString& str)
{
	if (size_t pos = this->find(str) != this->npos)
	{
		size_t n = str.d->len;
		return this->remove(pos, n);
	}
	else
	{
		throw EString("can not find str");
	}
}
EString	EString::sliced(size_t pos, size_t n) 
{
	return this->stringcutting(pos, n);
}

EString EString::stringcutting(size_t begin,size_t end)
{
	return EString(std::move(this->d->string_cut(begin, end)));
}
void EString::sort()
{

}
void EString::swap(EString& str)
{
	E_PRIVATE(EString)* p = this->d;
	this->d = str.d;
	str.d = p;
}
size_t EString::length() const 
{
	return this->d->len + 1;
}
bool EString::empty() const 
{
	if (this->d->len || this->d->Edata)
	{
		return false;
	}
	return true;
}

//运算符重载
EString& EString::operator+= (const EString& str)
{
	push_back(str);
	return *this;
}
EString& EString::operator+= (const char* ptr)
{
	push_back(ptr);
	return *this;
}
EString& EString::operator+= (wchar_t* ptr)
{
	push_back(ptr);
	return *this;
}
EString& EString::operator+= (char ptr)
{
	push_back(ptr);
	return *this;
}
EString& EString::operator= (const char* ptr)
{
	int len = strlen(ptr);
	this->d->Edata = (wchar_t*)malloc((len + 2) * sizeof(wchar_t));
	this->d->len = strlen(ptr) + 1;
	memcpy(this->d->Edata, ptr, len + 2);
	return *this;
}
EString& EString::operator= (const wchar_t* ptr)
{
	int len = wcslen(ptr);
	this->d->Edata = (wchar_t*)malloc((len + 2) * sizeof(wchar_t));
	this->d->len = len;
	memcpy(this->d->Edata, ptr, len + 2);
	this->d->Edata[len - 1] = '\0';
	this->d->Edata[len - 2] = '\0';
	return *this;
}
EString& EString::operator= (const EString& str)
{
	return this->operator=(str.d->Edata);
}
EString& EString::operator= (char ptr)
{
	this->d->Edata = (wchar_t*)malloc( sizeof(wchar_t)*2);
	this->d->len = 1;
	memcpy(this->d->Edata, &ptr, 1);
	this->d->Edata[1] = '\0';
	return *this;
}
wchar_t& EString::operator[] (size_t index)
{
	if (index >= this->d->len)
	{
		return this->d->Edata[index];
	}
	else
	{
		throw EString("out of range");
	}
}
EString EString::operator+(const EString& str) const
{
	EString temp(*this);
	return temp;
}
EString EString::operator+(const char* str) const
{
	EString temp(*this);
	return temp;
}
EString operator+(const char* str, const EString& other)
{
	EString temp(str);
	temp += other;
	return temp;
}
bool EString::operator==(EString& str)
{
	if (this->compare(str)==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool EString::operator==(const wchar_t& str)
{
	if (this->compare(str)==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool EString::operator==(const char& str)
{
	if (this->compare(str)==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool operator==(EString& str1, EString& str2)
{
	return str1.operator==(str2);
}
bool operator==(const wchar_t& str1, EString& str2)
{
	return !(str2 == str1);
}
bool operator==(const char& str1, EString& str2)
{
	return !(str2 == str1);
}
bool EString::operator!=(EString& str)
{
	return !(this->operator== (str));
}
bool EString::operator!=(const wchar_t& str)
{
	return !(this->operator== (str));
}
bool EString::operator!=(const char& str)
{
	return !(this->operator== (str));
}
bool operator!=(EString& str1, EString& str2)
{
	return !(str1.operator== (str2));
}
bool operator!=(const wchar_t& str1, EString& str2)
{
	return !(str2.operator== (str1));
}
bool operator!=(const char& str1, EString& str2)
{
	return !(str2.operator== (str1));
}
bool EString::operator<(EString& str)
{
	return this->operator<(str.d->Edata);
}
bool EString::operator<(const wchar_t* str)
{
	size_t len = wcslen(str);
	wchar_t* q = (wchar_t*)malloc(sizeof(wchar_t) * len);
	memcpy(q, str, len);
	for (wchar_t* p = this->d->Edata;
		p != nullptr || q != nullptr; p++, q++)
	{
		if (*p < *q)
		{
			return false;
		}
	}
	free(q);
	return true;
}
bool EString::operator<(const char* str)
{
	size_t len = strlen(str);
	wchar_t* q = (wchar_t*)malloc(sizeof(wchar_t) * len);
	mbstowcs(q, str, len);
	bool temp = this->operator<(q);
	free(q);
	return temp;
}
bool operator<(const wchar_t* str1, EString& str2)
{
	return str2 > str1;
}
bool operator<(const char* str1, EString& str2)
{
	return str2 > str1;
}
bool EString::operator>(EString& str)
{
	return this->operator>(str.d->Edata);
}
bool EString::operator>(const wchar_t* str)
{
	size_t len = wcslen(str);
	wchar_t* q = (wchar_t*)malloc(sizeof(wchar_t) * len);
	memcpy(q, str, len);
	for (wchar_t* p = this->d->Edata;
		p != nullptr || q != nullptr; p++, q++)
	{
		if (*p < *q)
		{
			return false;
		}
	}
	free(q);
	return true;
}
bool EString::operator>(const char* str)
{
	size_t len = strlen(str);
	wchar_t* q = (wchar_t*)malloc(sizeof(wchar_t) * len);
	mbstowcs(q, str, len);
	bool temp = this->operator>(q);
	free(q);
	return temp;
}
bool operator>(const wchar_t* str1, EString& str2)
{
	return str2 < str1;
}
bool operator>(const char* str1, EString& str2)
{
	return str2 < str1;
}
bool EString::operator<=(EString& str)
{
	return !(this->operator>(str));
}
bool EString::operator<=(const wchar_t* str) 
{
	return !(this->operator>(str));
}
bool EString::operator<=(const char* str)
{
	return !(this->operator>(str));
}
bool operator<=(const wchar_t* str1, EString& str2)
{
	return !(str1 > str2);
}
bool operator<=(const char* str1, EString& str2)
{
	return !(str1 > str2);
}
bool EString::operator>=(EString& str)
{
	return !(this->operator<(str));
}
bool EString::operator>=(const wchar_t* str)
{
	return !(this->operator<(str));
}
bool EString::operator>=(const char* str)
{
	return !(this->operator<(str));
}
bool operator>=(const wchar_t* str1, EString& str2)
{
	return !(str1 < str2);
}
bool operator>=(const char* str1, EString& str2)
{
	return !(str1 < str2);
}

//数字常量转换字符串
EString to_string(int value,int base)
{
	EString str;
	wchar_t* temp = nullptr;
	int count = 0;
	long long temp_value = value;
	while (temp_value /= base)
	{
		++count;
	}
	temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	count -= 1;
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)(value % base + 0x30);
		value /= base;
	}
	str = temp;
	free(temp);
	return str;
}
EString to_string(long value, int base)
{
	EString str;
	wchar_t* temp = nullptr;
	int count = 0;
	long long temp_value = value;
	while (temp_value /= base)
	{
		++count;
	}
	temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	count -= 1;
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)(value % base + 0x30);
		value /= base;
	}
	str = temp;
	free(temp);
	return str;
}
EString to_string(long long value, int base)
{
	EString str;
	wchar_t* temp = nullptr;
	int count = 0;
	while (value / base)
	{
		++count;
	}
	temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	count -= 1;
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)(value % base + 48);
		value /= base;
	}
	str = temp;
	free(temp);
	return str;
}
EString to_string(unsigned value, int base)
{
	EString str;
	wchar_t* temp = nullptr;
	int count = 0;
	while (value / base)
	{
		++count;
	}
	temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	count -= 1;
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)(value % base + 48);
		value /= base;
	}
	str = temp;
	free(temp);
	return str;
}
EString to_string(unsigned long value, int base)
{
	EString str;
	wchar_t* temp = nullptr;
	int count = 0;
	while (value / base)
	{
		++count;
	}
	temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	count -= 1;
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)(value % base + 48);
		value /= base;
	}
	str = temp;
	free(temp);
	return str;
}
EString to_string(unsigned long long value, int base)
{
	EString str;
	wchar_t* temp = nullptr;
	int count = 0;
	while (value / base)
	{
		++count;
	}
	temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	count -= 1;
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)(value % base + 48);
		value /= base;
	}
	str = temp;
	free(temp);
	return str;
}
EString to_string(float value, int base)
{
	EString str;
	long long int_f = (long long)value;//float to int
	str = to_string((long long)(int_f),base);
	int count = 0;
	float temp_main_value = value - int_f;
	float temp_self_value = temp_main_value;
	while (temp_self_value)
	{
		++count;
		temp_self_value *= base;
		temp_self_value -= (long long)temp_self_value;
	}

	wchar_t* temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)((long long)(temp_main_value * base) + 48);
		temp_main_value *= base;
	}
	str += temp;
	free(temp);
	return str;
}
EString to_string(double value,int base)
{
	EString str;
	long long int_f = (long long)value;//float to int
	str = to_string((long long)(int_f),base);
	int count = 0;
	float temp_main_value = value - int_f;
	float temp_self_value = temp_main_value;
	while (temp_self_value)
	{
		++count;
		temp_self_value *= base;
		temp_self_value -= (long long)temp_self_value;
	}

	wchar_t* temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)((long long)(temp_main_value * base) + 48);
		temp_main_value *= base;
	}
	str += temp;
	free(temp);
	return str;
}
EString to_string(long double value,int base)
{
	EString str;
	long long int_f = (long long)value;//float to int
	str = to_string((long long)int_f,base);
	int count = 0;
	float temp_main_value = value - int_f;
	float temp_self_value = temp_main_value;
	while (temp_self_value)
	{
		++count;
		temp_self_value *= base;
		temp_self_value -= (long long)temp_self_value;
	}

	wchar_t* temp = (wchar_t*)malloc(sizeof(wchar_t) * count);
	for (int i = 0; i < count; ++i)
	{
		temp[count - i] = (wchar_t)((long long)(temp_main_value * base) + 48);
		temp_main_value *= base;
	}
	str += temp;
	free(temp);
	return str;
}

std::wostream& operator<< (std::wostream& out, EString& str)
{
	std::wcout << str.d->Edata;
	while(str.d->Edata)
	return out;
}
std::ostream& operator<< (std::ostream& out, EString& str)
{
	char* temp = (char*)malloc(sizeof(char)*(str.d->len + 1));
	wcstombs(temp, str.d->Edata, str.d->len + 1);
	temp[str.d->len] = '\0';
	std::cout << temp;
	free(temp);
	return out;
}
std::wistream& operator>> (std::wistream& in, EString& str)
{
	wchar_t* p = nullptr;
	wcin >> p;
	str = p;
	return in;
}
std::istream& operator>> (std::istream& in, EString& str)
{
	char* p = nullptr;
	cin >> p;
	str = p;
	return in;
}
