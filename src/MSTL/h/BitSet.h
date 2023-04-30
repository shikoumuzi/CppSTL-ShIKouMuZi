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


	using EvalueFun = double(*)(UINT dna);//��Ӧ����������
	class BitSet
	{
	public:
		friend std::ostream &operator<<(std::ostream& out,BitSet& data);
		friend inline uint64_t operator|(uint64_t& other, const BitSet& data);
		friend inline uint64_t operator&(uint64_t& other, const BitSet& bitset);
	public://�������ܺ���
		
		BitSet(int);
		BitSet(int, int);
		BitSet(const BitSet&);
		BitSet(BitSet&&) noexcept;
		~BitSet();

	public://����λ����
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

	private://��������
#ifdef __BITSET_BINARY__
		static uint64_t getBit(uint64_t& data, size_t start, size_t len);//��ȡָ������ ָ��λ�ڵĶ������� ���ҹ�
		uint64_t getBit(size_t start, size_t len);//��ȡָ��λ��Χ�ڵĶ������� ���ҹ�
		size_t getBit(size_t index, size_t bit, int);//��ȡֵָ��λ�Ķ�������
		inline void analyzePos(size_t pos, size_t& index, size_t& bit);
		void setBit(size_t index, size_t bit, size_t value, int);//����ĳһλ��ֵ
		void setBit(size_t start, size_t end, uint64_t value);//����ĳһ�������ֵ Ҫ����һ�α����Լ��
		static inline uint64_t pow(int a);//��ȡȫΪ1������
#endif 
	public://����ģ��
#ifdef __BITSET_BINARY__
		void encode(int num);
#else
		void encode(uint64_t num);
#endif 
		void swap(BitSet& , size_t start, size_t end);
		void set();//����λ��Ϊ0
		void set(size_t value);//����λ��Ϊ1
		void set(size_t pos, size_t value);//��ĳһ��λ��Ϊvalue
		void flip(size_t pos);//��תĳһλ
		inline int size();
		

	public://ת��
		std::string toString();
#ifdef __BITSET_BINARY__
		static std::string toString(uint64_t other);
#endif // 
		std::shared_ptr<size_t[]> split(int bit_width, int len);

	public://�����ӿ�
		UINT evalueBit(size_t start, size_t len);//�ú����ᰴҪ����ȡ�������������������������λ

	private:
		class BitSetData* data;
	};

};
#endif // !__BITSET_H__
