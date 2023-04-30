#include"BitSet.h"
#ifdef __BITSET_ARRAY__
#include<cstring>
#include<string.h>
#include<random>
#include<stdlib.h>
namespace MUZI {
	std::mt19937_64 random_engine;
	std::uniform_int_distribution<> bitset_rand_num;

	class BitSetData
	{
	public:
		BitSetData(int size)
		{
			this->capacity = size * 2;
			this->num = (unsigned char*)malloc(sizeof(unsigned char) * this->capacity);
			this->size = size;
			this->start = this->num;
			this->end = this->start + size;
			memset(this->num, 0, size);
			this->start_pos = 0;
			this->end_pos = size;
		}
		BitSetData(const BitSetData& other)
		{
			this->operator=(other);
		}
		~BitSetData()
		{
			free(this->num);
			this->num = nullptr;
		}
	public:
		void operator=(const BitSetData& other)
		{
			free(this->num);
			this->num = (unsigned char*)malloc(sizeof(unsigned char) * other.capacity);
			memcpy(this->num, other.num, other.capacity);
			this->start = this->num + ((other.start - other.num) / sizeof(unsigned char));
			this->end = this->start + other.size;
			this->size = other.size;
			this->capacity = other.capacity;
		}
	public:
		unsigned char* num;
		unsigned char* start;
		unsigned char* end;
		size_t size;
		size_t capacity;
		int start_pos;
		int end_pos;
	};

	std::ostream& operator<<(std::ostream& out, BitSet& data);
	inline uint64_t operator|(uint64_t& other, const BitSet& data);
	inline uint64_t operator&(uint64_t& other, const BitSet& bitset);
//基础功能函数
	BitSet::BitSet(int size)
	{
		this->data = new BitSetData(size);
	}
<<<<<<< HEAD
	BitSet::BitSet(int, int)
	{}
=======
	BitSet::BitSet(int, int);
>>>>>>> 5270042c6f209941ee8c2fd9fce36cbb1eb7f638
	BitSet::BitSet(const BitSet& other)
	{
		this->data->operator=(*other.data);
	}
	BitSet::BitSet(BitSet&& other) noexcept
	{
		this->data = other.data;
		other.data = new BitSetData(other.data->size);
	}
	BitSet::~BitSet()
	{
		delete this->data;
	}
//基本位运算
	void BitSet::operator=(std::string& other)
	{
		int i = 0;
<<<<<<< HEAD
		for (auto it = other.rbegin(); it != other.rend() && i < this->data->size; ++it)
=======
		for (auto it = other.rbegin(); it != other.rend() && i < this->size; ++it)
>>>>>>> 5270042c6f209941ee8c2fd9fce36cbb1eb7f638
		{
			*(this->data->start + i) = *it - 48;
		}
	}
	void BitSet::operator=(uint8_t other)
	{
		memset(this->data->start, 0, this->data->size);
		this->encode((uint64_t)other);
	}
	void BitSet::operator=(BitSet& other)
	{
		this->data->operator=(*other.data);
	}
	void BitSet::operator=(BitSet&& other)
	{
		this->data = other.data;
		other.data = new BitSetData(other.data->size);
	}
	BitSet BitSet::operator~()
	{
		BitSet ret(*this);
		for (int i = 0; i < this->data->size; ++i)
		{
			ret.data->start[i] = ~ret.data->start[i];
			ret.data->start[i] %= 2;
		}
		return ret;
	}
	BitSet BitSet::operator+(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		bitset_tmp += bitset_tmp;
		return bitset_tmp;
	}
	BitSet BitSet::operator-(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		bitset_tmp -= bitset_tmp;
		return bitset_tmp;
	}
	BitSet BitSet::operator*(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		bitset_tmp *= bitset_tmp;
		return bitset_tmp;
	}
	BitSet BitSet::operator/(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		bitset_tmp /= bitset_tmp;
		return bitset_tmp;
	}
	BitSet BitSet::operator+(const BitSet& other)
	{
		BitSet ret(*this);
		ret += other;
		return ret;
	}
	BitSet BitSet::operator-(const BitSet& other)
	{
		BitSet ret(*this);
		ret -= other;
		return ret;
	}
	BitSet BitSet::operator*(const BitSet& other)
	{
		BitSet ret(*this);
		ret *= other;
		return ret;
	}
	BitSet BitSet::operator/(const BitSet& other)
	{
		BitSet ret(*this);
		ret /= other;
		return ret;
	}
	void BitSet::operator+=(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		this->operator+=(bitset_tmp);
	}
	void BitSet::operator-=(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		this->operator-=(bitset_tmp);
	}
	void BitSet::operator*=(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		this->operator*=(bitset_tmp);
	}
	void BitSet::operator/=(uint8_t& other)
	{
		BitSet bitset_tmp(this->data->capacity);
		bitset_tmp.encode(other);
		this->operator/=(bitset_tmp);
	}
	void BitSet::operator+=(const BitSet& other)
	{
		int co_1 = 0, co_2 = 0, co_3 = 0, co_4 = 0;//co_1 为加数被加数的和的进位信息， co_2是进位信息传入后同和得出的进位信息， co_3是进位结果, co_4传入的进位信息
		int sum_1 = 0, sum_2 = 0;//加数和被加数为sum_1 进位信息和sum_1 为sum_2
		for (int i = 0; i < this->data->size; ++i)
		{
			sum_1 = this->data->start[i] ^ other.data->start[i];
			sum_2 = sum_1 ^ co_4;
			co_1 = this->data->start[i] & other.data->start[i];
			co_2 = co_4 & sum_1;
			co_3 = co_1 | co_2;
			co_4 = co_3;
			this->data->start[i] = sum_2;
		}
	}
	void BitSet::operator-=(const BitSet& other)
	{
		BitSet reversal_other(other);
		reversal_other = ~reversal_other;
<<<<<<< HEAD
		//reversal_other += (uint8_t)1;
=======
		reversal_other += 1;
>>>>>>> 5270042c6f209941ee8c2fd9fce36cbb1eb7f638
		this->operator+=(reversal_other);
	}
	void BitSet::operator*=(const BitSet& other)
	{

	}
	void BitSet::operator/=(const BitSet& other)
	{

	}
	BitSet BitSet::operator<<(size_t len)
	{
		BitSet ret(*this);
		ret <<= len;
		return ret;
	}
	void BitSet::operator<<=(size_t len)
	{
		if (this->data->start - len > this->data->num)
		{
			memset(this->data->start - len, 0, len);
			memset(this->data->end - len, 0, len);
			this->data->start -= len;
		}
		else
		{

		}
	}
	BitSet BitSet::operator>>(size_t len)
	{
		BitSet ret(*this);
		ret >>= len;
		return ret;
	}
	void BitSet::operator>>=(size_t len)
	{
		if (this->data->end + len < this->data->num + this->data->capacity)
		{
			memset(this->data->end, 0, len);
			memset(this->data->start, 0, len);
			this->data->start -= len;
		}
		else
		{

		}
	}
	BitSet BitSet::operator|(BitSet& other)
	{
		BitSet ret(*this);
		ret |= other;
		return ret;
	}
	void BitSet::operator|=(BitSet& other)
	{
		for (int i = 0; i < this->data->size; ++i)
		{
			*(this->data->start + i) |= *(other.data->start + i);
		}
	}
	BitSet BitSet::operator&(BitSet& other)
	{
		BitSet ret(*this);
		ret &= other;
		return ret;
	} 
	void BitSet::operator&=(BitSet& other)
	{
		for (int i = 0; i < this->data->size; ++i)
		{
			*(this->data->start + i) &= *(other.data->start + i);
		}
	}
	size_t BitSet::operator[](size_t pos)
	{
		return *(this->data->start + pos);
	}
	bool BitSet::operator==(BitSet& other)
	{
		for (int i = this->data->size - 1; i >= 0 ; --i)
		{
			if (*(this->data->start + i) != *(other.data->start + i))
			{
				return false;
			}
		}
		return true;
	}
	bool BitSet::operator!=(BitSet& other)
	{
		return !this->operator==(other);
	}
	bool BitSet::operator<(BitSet& other)
	{
<<<<<<< HEAD
		return !this->operator>=(other);
=======
		return !this->operator>=(other)
>>>>>>> 5270042c6f209941ee8c2fd9fce36cbb1eb7f638
	}
	bool BitSet::operator<=(BitSet& other)
	{
		for (int i = this->data->size - 1; i >= 0; --i)
		{
			if (*(this->data->start + i) > *(other.data->start + i))
			{
				return false;
			}
		}
		return true;
	}
	bool BitSet::operator>(BitSet& other)
	{
		return !this->operator<=(other);
	}
	bool BitSet::operator>=(BitSet& other)
	{
		for (int i = this->data->size - 1; i >= 0; --i)
		{
			if (*(this->data->start + i) < *(other.data->start + i))
			{
				return false;
			}
		}
		return true;
	}

//功能模块
	void BitSet::encode(uint64_t num)
	{
		for (int i = 0; i < this->data->size; ++i)
		{
			*(this->data->start + i) = num % 2;
			num >>= 1;
		}
	}
	void BitSet::swap(BitSet& other, size_t start, size_t end)
	{
		size_t len = end - start;
		unsigned char* buff = new unsigned char[len];
		memcpy(buff, other.data->start + start, len);
		memcpy(other.data->start + start, this->data->start + start, len);
		memcpy(this->data->start + start, buff, len);
		delete[] buff;
	}
	void BitSet::set()//所有位置为0
	{
		memset(this->data->start, 0, this->data->size);
	}
	void BitSet::set(size_t value)//所有位置为1
	{
		memset(this->data->start, 1, this->data->size);
	}
	void BitSet::set(size_t pos, size_t value)//将某一个位置为value
	{
		*(this->data->start + pos) = (value > 0) ? 1 : 0;
	}
	void BitSet::flip(size_t pos)//反转某一位
	{
		*(this->data->start + pos) = ~*(this->data->start + pos);
		*(this->data->start + pos) %= 2;
	}
	int BitSet::size()
	{
		return this->data->size;
	}

//转换
	std::string BitSet::toString()
	{
		std::string ret;
		ret.reserve(this->data->size);
		for (int i = 0; i < this->data->size; ++i)
		{
			ret += *(this->data->start + i) + 48;
		}
<<<<<<< HEAD
		return ret;
	} 
	std::shared_ptr<size_t[]> BitSet::split(int bit_width, int len)
	{
		std::shared_ptr<size_t[]> ret;
		return ret;
	}

//评估接口
	UINT BitSet::evalueBit(size_t start, size_t len) { return 0; }//该函数会按要求提取出二进制数，并且右移置最低位
=======
	} 
	std::shared_ptr<size_t[]> BitSet::split(int bit_width, int len)
	{

	}

//评估接口
	UINT BitSet::evalueBit(size_t start, size_t len) {}//该函数会按要求提取出二进制数，并且右移置最低位
>>>>>>> 5270042c6f209941ee8c2fd9fce36cbb1eb7f638

};



#endif // __BITSET_ARRAY__
