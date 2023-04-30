#ifndef __BITSET_H__
#define __BITSET_H__
#pragma once

#define __BITSET_ARRAY__
// you can define __BITSET_BINARY__ and close __BITSET_ARRAY__ to let the BitSet_Binary work

#include<stdint.h>
#include<string>
#include<vector>
#include<iostream>
#include <bitset>
#define UINT uint32_t
namespace MUZI {


	using EvalueFun = double(*)(UINT dna);//适应度评估函数
	class BitSet
	{
	public:
		friend std::ostream &operator<<(std::ostream& out,BitSet& data);
		friend inline uint64_t operator|(uint64_t& other, const BitSet& data);
		friend inline uint64_t operator&(uint64_t& other, const BitSet& bitset);
	public://基础功能函数
		
		BitSet(int);
		BitSet(int, int);
		BitSet(const BitSet&);
		BitSet(BitSet&&) noexcept;
		~BitSet();

	public://基本位运算
		void operator=(std::string&);
		void operator=(uint8_t);
		void operator=(BitSet&);
		void operator=(BitSet&&);
#ifdef __BITSET_ARRAY__
		BitSet operator~();
		BitSet operator+(uint8_t&);
		BitSet operator+(const BitSet&);
		BitSet operator-(uint8_t&);
		BitSet operator-(const BitSet&);
		BitSet operator*(uint8_t&);
		BitSet operator*(const BitSet&);
		BitSet operator/(uint8_t&);
		BitSet operator/(const BitSet&);
		void operator+=(uint8_t&);
		void operator+=(const BitSet&);
		void operator-=(uint8_t&);
		void operator-=(const BitSet&);
		void operator*=(uint8_t&);
		void operator*=(const BitSet&);
		void operator/=(uint8_t&);
		void operator/= (const BitSet&);
#endif // __BITSET_ARRAY__
		void operator+=(size_t);
		BitSet operator<<(size_t);
		void operator<<=(size_t);
		BitSet operator>>(size_t);
		void operator>>=(size_t);
		BitSet operator|(BitSet&);
		void operator|=(BitSet&);
		BitSet operator&(BitSet&);
		void operator&=(BitSet&);
		size_t operator[](size_t bit);
		bool operator==(BitSet&);
		bool operator!=(BitSet&);
		inline bool operator<(BitSet&);
		bool operator<=(BitSet&);
		inline bool operator>(BitSet&);
		bool operator>=(BitSet&);

	private://辅助功能
#ifdef __BITSET_BINARY__
		static uint64_t getBit(uint64_t& data, size_t start, size_t len);//获取指定数的 指定位内的二进制数 不右归
		uint64_t getBit(size_t start, size_t len);//获取指定位范围内的二进制数 不右归
		size_t getBit(size_t index, size_t bit, int);//获取值指定位的二进制数
		inline void analyzePos(size_t pos, size_t& index, size_t& bit);
		void setBit(size_t index, size_t bit, size_t value, int);//设置某一位的值
		void setBit(size_t start, size_t end, uint64_t value);//设置某一个区间的值 要求这一段必须规约好
		static inline uint64_t pow(int a);//获取全为1的序列
#endif 
	public://功能模块
#ifdef __BITSET_BINARY__
		void encode(int num);
#else
		void encode(uint64_t num);
#endif 
		void swap(BitSet& , size_t start, size_t end);
		void set();//所有位置为0
		void set(size_t value);//所有位置为1
		void set(size_t pos, size_t value);//将某一个位置为value
		void flip(size_t pos);//反转某一位
		inline int size();
		

	public://转换
		std::string toString();
#ifdef __BITSET_BINARY__
		static std::string toString(uint64_t other);
#endif // 
		std::shared_ptr<size_t[]> split(int bit_width, int len);

	public://评估接口
		UINT evalueBit(size_t start, size_t len);//该函数会按要求提取出二进制数，并且右移置最低位

	private:
		class BitSetData* data;
	};

};
#endif // !__BITSET_H__
