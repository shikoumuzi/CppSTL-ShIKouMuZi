#include"BitSet.h"
#ifdef __BITSET_BINARY__


#include<stdlib.h>
#include<time.h>
#include<random>
#include<cmath>
#include <iomanip> 

#include<vector>
namespace MUZI {

	std::mt19937_64 random_engine;
	std::uniform_int_distribution<> bitset_rand_num;//随机数发生器 决定变异位数
	
	static struct PowData
	{
		PowData()
		{
			uint64_t tmp = 0;
			this->_index_ = new uint64_t[64];
			for (int i = 0; i < 64; ++i)
			{
				tmp += 1;
				this->_index_[i] = tmp;
				tmp <<= 1;
			}
		}
		~PowData()
		{
			delete[] this->_index_;
		}
		uint64_t operator[](int pos)
		{
			return this->_index_[pos];
		}
		uint64_t* _index_;
	}pow_data;

	class BitSetData 
	{
	public:
		//存储规则为 低位数 的下标 在前， 高位数 的下标 在后
		BitSetData() {}
		BitSetData(int bit_len)
		{
			this->len = (int)((bit_len - 1) / 64) + 1;
			this->num.resize((this->len + 1), 0);
		}
		BitSetData(int bit_len, int num):BitSetData(bit_len)
		{
			for (int i = 0; i < this->len; ++i)
			{

				this->num[i] += bitset_rand_num(random_engine) % num;
				
			}
		}
		BitSetData(const BitSetData& other)
		{
			this->len = other.len;
			this->num = other.num;
		}
		~BitSetData()
		{}
	public:
		size_t len;
		std::vector<uint64_t> num;
	};

	//基本类功能
	BitSet::BitSet(int bit_len)
	{
		this->data = new BitSetData(bit_len);
	}
	BitSet::BitSet(int bit_len, int num)
	{
		this->data = new BitSetData(bit_len, num);
	}
	BitSet::BitSet(const BitSet& other)
	{
		this->data = new BitSetData(*other.data);
	}
	BitSet::BitSet(BitSet&& other) noexcept
	{
		this->data = other.data;
		other.data = nullptr;
	}
	BitSet::~BitSet()
	{
		delete this->data;
	}

	//辅助功能
	uint64_t BitSet::getBit(uint64_t& data, size_t start, size_t len)//结果 为 000000 111010100 0000
	{
		uint64_t ret = 0;
		size_t bit =  start % 64;
		ret = BitSet::pow(len);
		ret <<= bit;
		ret &= data;
		ret >>= bit;
		return ret;
	}
	uint64_t BitSet::getBit(size_t start, size_t len)
	{
		if (len >= 64)
		{
			return static_cast<uint64_t>(-1);
		}
		size_t index_1 = 0, bit_1 = 0;
		size_t index_2 = 0, bit_2 = 0;
		this->analyzePos(start, index_1, bit_1);
		this->analyzePos(start + len, index_2, bit_2);
		uint64_t ret = 0;

		ret |= pow(len);
		ret <<= bit_1;
		ret &= this->data->num[index_1];
		ret >>= bit_1;
		
		//由于位数限制每次寻址只能返回64位数
		//else
		//{
		//	size_t temp_end = bit_1;
		//	size_t temp_start = 64 - bit_2;//这两个代表边界距离
		//	size_t temp_start_distance = temp_start;
		//	uint64_t ret_high = 0, ret_low = 0;

		//	ret_high <<= temp_end;
		//	ret_high |= this->pow(temp_end);
		//	ret_high &= this->data->num[index_2];
		//	ret_high <<= temp_start;

		//	ret_low <<= temp_start;
		//	ret_low |= this->pow(temp_start);
		//	ret_low <<= bit_2;
		//	ret_low &= this->data->num[index_1];
		//	ret_low >>= bit_2;
		//	ret = ret_high + ret_low;
		//}
		return ret;
	}
	size_t BitSet::getBit(size_t index, size_t bit, int)//获取值指定位的二进制数
	{
		uint64_t zero = ~(1 << bit);
		zero |= this->data->num[index];
		zero >>= (bit - 1);
		return static_cast<size_t>((~zero == 0) ? 1: 0);
	}
	void BitSet::analyzePos(size_t pos, size_t& index, size_t& bit)
	{
		bit = pos % 64;
		index = pos / 64;
	}
	void BitSet::setBit(size_t index,size_t bit, size_t value, int)
	{
		uint64_t num = 0;
		uint64_t once = static_cast<uint64_t>(-1);
		num <<= (64 - (bit + 1) - 1);
		num |= this->pow(64 - (bit + 1) - 1);
		num <<= 1;
		num += value;
		num << (bit - 1);
		num |= this->pow(bit - 1);
		this->data->num[index] &= num;
	}
	void BitSet::setBit(size_t start, size_t end, uint64_t value)
	{
		size_t len = end - start;
		size_t index_1 = 0, bit_1 = 0;
		size_t index_2 = 0, bit_2 = 0;
		this->analyzePos(start, index_1, bit_1);
		this->analyzePos(end, index_2, bit_2);

		uint64_t once_to_set1 = 0;
		//构造 1111 0000000000 111111111将 num 中需要交换的段清空
		once_to_set1 |= this->pow(bit_2 - bit_1);// 0000 1111111
		once_to_set1 <<= bit_1;// 00000 1111111 000
		this->data->num[index_1] |= once_to_set1;
		value <<= (bit_1);
		value |= ~once_to_set1; // 11111 data 111111
		this->data->num[index_1] &= value;
		
		//else
		//{
		//	uint64_t high_bit = value >> (len - bit_2);
		//	uint64_t low_bit = value << bit_2;

		//	uint64_t once_to_set1 = static_cast<uint64_t>(-1);
		//	uint64_t second_to_set1 = static_cast<uint64_t>(-1);

		//	once_to_set1 << (this->pow(bit_2));
		//	second_to_set1 << (this->pow(64 - bit_1));

		//	this->data->num[index_2] |= once_to_set1;
		//	this->data->num[index_2] &= high_bit;

		//	this->data->num[index_1] |= second_to_set1;
		//	this->data->num[index_1] &= low_bit;
		//}
	}
	uint64_t BitSet::pow(int b)
	{//等比数列求和
		return (uint64_t)pow_data[b];
		//(1 * (std::pow(2, b) - 1))
		//如果 采用double - 1 后在直接转uint64_t 会自动四舍五入 进而导致计算错误
	}

	//基本位运算
	void BitSet::operator=(std::string& other)
	{
		if (this->data == nullptr)
		{
			throw std::string("not init");
			return;
		}
		for (auto it = other.rbegin(); it != other.rend(); ++it)
		{
			this->operator<<(1);
			this->operator+=((*it - 48));
		}
	}
	void BitSet::operator=(BitSet& other)
	{
		if (this->data == nullptr)
		{
			this->data = new BitSetData;
		}
		this->data->len = other.data->len;
		this->data->num = other.data->num;
	}
	void BitSet::operator=(BitSet&& other)
	{
		if (this->data != nullptr)
		{
			delete this->data;
		}
		this->data = other.data;
		other.data = nullptr;
	}

	void BitSet::operator+=(size_t value)
	{
		this->data->num[0] += value;
	}
	BitSet BitSet::operator<<(size_t opr_num)
	{
		BitSet ret(*this);
		ret <<= opr_num;
		return ret;
	}
	void BitSet::operator<<=(size_t opr_num)
	{
		uint64_t zero = 0;
		uint64_t one = static_cast<uint64_t>(-1);
		uint64_t temp = 0;
		for (int i = this->data->len; i > 0; --i)
		{
			this->data->num[i] <<= opr_num;
			temp = (this->getBit(this->data->num[i - 1], 64 - opr_num, opr_num));
			this->data->num[i] += temp;
		}
		this->data->num[0] <<= opr_num;
	}
	BitSet BitSet::operator>>(size_t opr_num)
	{
		BitSet ret(*this);
		ret >>= opr_num;
		return ret;
	}
	void BitSet::operator>>=(size_t opr_num)
	{
		uint64_t zero = 0;
		uint64_t one = static_cast<uint64_t>(-1);
		uint64_t temp = 0;
		for (int i = 0; i < this->data->len; ++i)
		{
			this->data->num[i] >>= opr_num;
			temp = this->getBit(this->data->num[i + 1], 0, opr_num) << (64 - opr_num);
			this->data->num[i] += temp;
		}
		this->data->num[this->data->len] >>= opr_num;
	}
	BitSet BitSet::operator|(BitSet& other)
	{
		BitSet ret(*this);
		ret |= other;
		return ret;
	}
	void BitSet::operator|=(BitSet& other)
	{
		for (int i = 0; i < this->data->len; ++i)
		{
			this->data->num[i] |= other.data->num[i];
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
		for (int i = 0; i < this->data->len; ++i)
		{
			this->data->num[i] &= other.data->num[i];
		}
	}
	size_t BitSet::operator[](size_t pos)
	{
		size_t index = 0, bit = 0;
		this->analyzePos(pos, index, bit);
		return this->getBit(index, bit, 0);
	}
	bool BitSet::operator==(BitSet& other)
	{
		return !this->operator!=(other);
	}
	bool BitSet::operator!=(BitSet& other)
	{
		for (int i = 0; i < this->data->len; ++i)
		{
			if (this->data->num[i] != other.data->num[i])
			{
				return true;
			}
		}
		return false;
	}
	bool BitSet::operator<(BitSet& other)
	{
		for (int i = this->data->len - 1; i >= 0; --i)
		{
			if (this->data->num[i] >= other.data->num[i])
			{
				return false;
			}
		}
		return true;
	}
	bool BitSet::operator<=(BitSet& other)
	{
		return !this->operator>(other);
	}
	bool BitSet::operator>(BitSet& other)
	{
		for (int i = this->data->len - 1; i >= 0; --i)
		{
			if (this->data->num[i] <= other.data->num[i])
			{
				return false;
			}
		}
		return true;
	}
	bool BitSet::operator>=(BitSet& other)
	{
		return !this->operator<(other);
	}

	//功能模块
	void BitSet::encode(int num)
	{
		for (int i = 0; i < this->data->len; ++i)
		{
			this->data->num[i] = bitset_rand_num(random_engine)% num;
		}
	}
	void BitSet::swap(BitSet& other, size_t start, size_t end)
	{
		
		size_t index_1 = 0, pos_1 = 0;
		size_t index_2 = 0, pos_2 = 0;
		this->analyzePos(start, index_1, pos_1);
		this->analyzePos(end, index_2, pos_2);

		if (index_1 == index_2)
		{

			uint64_t selfdna = this->getBit(start, end - start);
			uint64_t otherdna = other.getBit(start, end - start);
			this->setBit(start, end, (uint64_t)otherdna);
			other.setBit(start, end, (uint64_t)selfdna);
		}
		else
		{
			uint64_t selfdna_1 = 0, selfdna_2 = 0;
			uint64_t otherdna_1 = 0, otherdna_2 = 0;
			size_t end_start = int(end / 64) * 64;
			selfdna_1 = this->getBit(start, 64 - start);
			selfdna_2 = this->getBit(end_start, end - end_start);
			otherdna_1 = other.getBit(start, 64 - start);
			otherdna_2 = other.getBit(end_start, end - end_start);
			this->setBit(start, 63, otherdna_1);
			this->setBit(end_start, end, otherdna_2);
			other.setBit(start, 63, selfdna_1);
			other.setBit(end_start, end, selfdna_2);
		}
	}
	void BitSet::set()//所有位置为0
	{
		for (int i = 0; i < this->data->len; ++i)
		{
			this->data->num[i] = 0;
		}
	}
	void BitSet::set(size_t value)
	{
		this->data->num.resize(this->data->num.size(), static_cast<uint64_t>(-1));
	}
	void BitSet::set(size_t pos, size_t value)//将某一个位置为value
	{
		size_t index = 0, bit = 0;
		this->analyzePos(pos, index, bit);
		this->setBit(index, bit, value, 0);
	}
	void BitSet::flip(size_t pos)//反转某一位
	{
		size_t ret = 0;
		size_t index = 0, bit = 0;
		this->analyzePos(pos, index, bit);
		if ((ret = this->getBit(index, bit, 0)))
		{
			this->setBit(index, bit, (size_t)0, 0);
		}
		else
		{
			this->setBit(index, bit, (size_t)1, 0);
		}
		
	}
	int BitSet::size()
	{
		return (this->data->len + 1) * 64;
	}

	//转换
	std::string BitSet::toString()
	{
		char char_str[1000] = { "\0" };
		std::string string_str;
		for (int i = this->data->len - 1; i >= 0; --i)
		{
			_i64toa_s(this->data->num[i], char_str, 1000, 2);
			string_str += char_str;
			string_str += "\t";
		}
		return string_str;
	}	
	std::string BitSet::toString(uint64_t other)
	{
		char char_str[1000] = { "\0" };
		_i64toa_s(other, char_str, 1000, 2);
		std::string string_str(char_str);
		return string_str;
	}
	std::shared_ptr<size_t[]> BitSet::split(int bit_width, int len)
	{
		//返回数据
		std::shared_ptr<size_t[]> ret(new size_t[len], [](size_t* ptr) {delete[] ptr; });
		bit_width -= 1;
		//复制临时数据
		BitSet temp_data(*this);

		//用以下列操作使用的数
		uint64_t num = 0;
		num = this->pow(bit_width);
		uint64_t num_temp = 0;

		for (int i = 0; i < len; ++i)
		{
			num_temp = num;
			//或与符号区分 要取数需要的是与
			ret[i] = num_temp & temp_data;
			temp_data >>= bit_width;
		}
		return ret;
	}

	std::ostream& operator<<(std::ostream& out, BitSet& data)
	{
		std::string tmp_str = data.toString();
		std::cout << std::setiosflags(std::ios::right)<<std::setw(data.data->len * 70) <<tmp_str << " " << tmp_str.size();
		return out;
	}
	uint64_t operator|(uint64_t& other, const BitSet& bitset)
	{
		return other | bitset.data->num[0];
	}
	uint64_t operator&(uint64_t& other, const BitSet& bitset)
	{
		return other & bitset.data->num[0];
	}
	
};
#endif // DEBUG

//bitset问题
//左右移只移动一个 uint64_T