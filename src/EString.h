#pragma once

#include<iostream>
#include<stdlib.h>
//#include<stddef.h>
#include<string.h>
#include "assert.h"
#include"EuiOpthon.h"
#include<iomanip>
#define E_PRIVATE(classname) classname##Private

class EString
{
private:
	E_PRIVATE(EString)* d;
	static constexpr auto npos{ static_cast<size_t>(-1) };//����һ�������ڵ����ֵ
public:
	//������
	friend class Iterator;
	class Iterator
	{
	public:
		friend class EString;
	private:
		E_PRIVATE(Iterator)* it;
	public:
		Iterator();
		Iterator(EString& base);
		Iterator(EString::Iterator& rhs);
		Iterator(EString::Iterator&& rhs);
		Iterator(const EString::Iterator& rhs);

	public:
		wchar_t operator*();
		bool operator==(EString::Iterator& rhs);
		bool operator!=(EString::Iterator& rhs);
		bool operator<(EString::Iterator& rhs);
		bool operator>(EString::Iterator& rhs);
		bool operator>=(EString::Iterator& rhs);
		bool operator<=(EString::Iterator& rhs);
		EString::Iterator& operator++();
		const EString::Iterator operator++(int);
		EString::Iterator& operator--();
		const EString::Iterator operator--(int);
		EString::Iterator& operator=(const EString::Iterator& rhs);
	};
public:
	EString();
	EString(EString& other);
	EString(EString&& other) noexcept;
	EString(const EString& other);
	EString(const char* other);
	EString(const wchar_t* other);
	EString(wchar_t&& other);
	EString(char* other);
	EString(char other);
	EString(EString::Iterator& begin, EString::Iterator& end);
	EString(EString&str, size_t begin, size_t end);
	~EString();
public:
	//��ʽ�����
	EString	arg(const EString& a/* = QLatin1Char(' ') */) const;
	EString	arg(long long a/*, int fieldWidth = 0, */,int base = 10/* char fillChar = ' '/* = QLatin1Char(' ') */) const;
	EString	arg(unsigned long long a,/* int fieldWidth = 0, */int base = 10/*, char fillChar = ' ' /* = QLatin1Char(' ') */) const;
	EString	arg(long a, /*int fieldWidth = 0*/int base = 10/*, char fillChar = ' ' /* = QLatin1Char(' ') */) const;
//	EString	arg(long a, int fieldWidth = 0, int base = 10, char fillChar = ' ' /* = QLatin1Char(' ') */) const;
	EString	arg(int a, /*int fieldWidth = 0,*/int base = 10/*, char fillChar = ' '/* = QLatin1Char(' ') */) const;
//	EString	arg(int a, int fieldWidth = 0, int base = 10, char fillChar = ' '/* = QLatin1Char(' ') */) const;
	EString	arg(short a, /*int fieldWidth = 0,*/ int base = 10/*, char fillChar = ' '/* = QLatin1Char(' ') */) const;
	EString	arg(unsigned short a/*, int fieldWidth = 0,*/, int base = 10/*, char fillChar = ' '/* = QLatin1Char(' ') */) const;
	EString	arg(double a /*int fieldWidth = 0,*/ /*char format = 'g', int precision = -1, char fillChar = ' '/* = QLatin1Char(' ') */) const;
	EString	arg(char a /*int fieldWidth = 0,*//* char fillChar = ' '/* = QLatin1Char(' ') */) const;
//	EString	arg(char a, int fieldWidth = 0, char fillChar = ' '/* = QLatin1Char(' ') */) const; 
	template<typename...Args>
	EString	arg(Args&&... args) const;
	//����ɾ��
	EString::Iterator begin();
	EString::Iterator end();
	EString::Iterator rbegin();
	EString::Iterator rend();
	EString at(size_t index);
	EString& append(const EString& str);
	EString& append(char ch);
	EString& append(const char* str, size_t len);
	EString& append(const char* str);
	EString::Iterator earse(EString::Iterator& begin);
	EString::Iterator earse(EString::Iterator& begin, EString::Iterator& end);
	EString& insert(EString::Iterator& begin, EString& other);
	EString& insert(EString::Iterator& begin, EString&& other);
	EString& insert(EString::Iterator& begin, const char* other);
	EString& insert(EString::Iterator& begin, char* other);
	EString& insert(EString::Iterator& begin, char other);
	EString& insert(EString::Iterator& begin, const wchar_t* other);
	EString& insert(EString::Iterator& begin, wchar_t* other);
	EString& insert(EString::Iterator& begin, wchar_t other);
	EString first(size_t size);
	EString& fill(char ch, size_t size = -1);
	size_t find(EString& str,size_t begin =0,size_t end = 0);
	size_t find(const EString& str,size_t index = 0, size_t end = 0);
	size_t find(char* str, size_t index = 0, size_t end = 0);
	size_t find(const char * str, size_t index = 0, size_t end = 0);
	size_t find(wchar_t* str, size_t index = 0, size_t end = 0);
	size_t find(const wchar_t* str, size_t index = 0, size_t end = 0);
	bool startsWith(const EString& s) const;
	bool startsWith(char* c) const;
	bool startsWith(wchar_t* c) const;
	bool endsWith(const EString& s) const;
	bool endsWith(char* c) const;
	bool endsWith(wchar_t* c) const;
	EString& replace(size_t position, size_t n, const EString& after);
	EString& replace(size_t position, size_t n, char after);
	EString& replace(size_t position, size_t n, const char* unicode, size_t size);
	EString& replace(size_t position, size_t n, const wchar_t* after);
	EString& replace(char before, char after);
	EString& replace(const char* before, size_t blen, const char* after, size_t alen);
	EString& replace(const EString& before, const EString& after);
	EString& replace(char ch, const EString& after);
	EString	repeated(size_t times) const;
	void reserve(size_t size);
	void resize(size_t size);
	void resize(size_t size, char fillChar);
	EString	right(size_t n) const;
	EString& remove(size_t position, size_t n);
	EString& remove(EString::Iterator& begin, EString::Iterator& end);
	EString& remove(char ch);
	EString& remove(char* str);
	EString& remove(wchar_t* str);
	EString& remove(const EString& str);
	EString	sliced(size_t pos, size_t n = 0);
	EString stringcutting(size_t begin, size_t end);
	size_t length() const;
	bool empty() const;
	void sort();
	void swap(EString& str);
public:
	//push/pop
	void push_back(const EString& other);
	void push_back(const char& other);
	void push_back(char* const other);
	void push_back(wchar_t* other);
	void push_back(wchar_t other);
	void push_front(const EString& other);
	void push_front(char* const other);

	//����
	int	compare(const EString& other) const;
	int	compare(const char* other) const;
	int	compare(const wchar_t* other) const;
	wchar_t back() const;
	wchar_t& back();
	void clear();
	size_t size() const;
	char* c_str() const;
	wchar_t* data() const;
public:
	EString& operator+= (const EString& str);
	EString& operator+= (const char* ptr);
	EString& operator+= (wchar_t* ptr);
	EString& operator+= (char ptr);
	EString& operator= (char ptr);
	EString& operator= (const char* ptr);
	EString& operator= (const wchar_t* ptr);
	EString& operator= (const EString& str);
	wchar_t& operator[] (size_t index);
	EString operator+(const EString& str) const;
	EString operator+(const char* str) const;
	friend EString operator+(const char* str,const EString& other);
	bool operator==(EString& str);
	bool operator==(const wchar_t& str);
	bool operator==(const char& str);
	bool operator!=(EString& str);
	bool operator!=(const wchar_t& str);
	bool operator!=(const char& str);
	bool operator<(EString& str);
	bool operator<(const wchar_t* str);
	bool operator<(const char* str);
	bool operator>(EString& str);
	bool operator>(const wchar_t* str);
	bool operator>(const char* str);
	bool operator<=(EString& str);
	bool operator<=(const wchar_t* str);
	bool operator<=(const char* str);	
	bool operator>=(EString& str);
	bool operator>=(const wchar_t* str);
	bool operator>=(const char* str);
	friend bool operator==(EString& str1, EString& str2);
	friend bool operator==(const wchar_t& str1, EString& str2);
	friend bool operator==(const char& str1, EString& str2);
	friend bool operator!=(EString& str1, EString& str2);
	friend bool operator!=(const wchar_t* str1, EString& str2);
	friend bool operator!=(const char* str1, EString& str2);
	friend bool operator<(const wchar_t* str1, EString& str2);
	friend bool operator<(const char* str1, EString& str2);
	friend bool operator>(const wchar_t* str1, EString& str2);
	friend bool operator>(const char* str1, EString& str2);
	friend bool operator<=(const wchar_t* str1, EString& str2);
	friend bool operator<=(const char* str1, EString& str2);
	friend bool operator>=(const wchar_t* str1, EString& str2);
	friend bool operator>=(const char* str1, EString& str2);



public:
	friend EString to_string(int value,int base);
	friend EString to_string(long value ,int base);
	friend EString to_string(long long value, int base);
	friend EString to_string(unsigned value,int base);
	friend EString to_string(unsigned long value, int base);
	friend EString to_string(unsigned long long value,int base);
	friend EString to_string(float value,int base);
	friend EString to_string(double value,int base);
	friend EString to_string(long double value,int base);

public:
	friend std::wostream& operator<< (std::wostream& out, EString& str);
	friend std::ostream& operator<< (std::ostream& out, EString& str);
	friend std::wistream& operator>> (std::wistream& in, EString& str);
	friend std::istream& operator>> (std::istream& in, EString& str);

#ifdef E_WINDOWS
	friend class EString;
#endif
};

EString to_string(int value,int base=10);
EString to_string(long value,int base =10);
EString to_string(long long value,int base =10);
EString to_string(unsigned value , int base=10);
EString to_string(unsigned long value , int base =10);
EString to_string(unsigned long long value,int base =10);
EString to_string(float value,int base =10);
EString to_string(double value,int base =10);
EString to_string(long double value,int base =10);

bool operator==(EString& str1, EString& str2);
bool operator==(const wchar_t& str1,EString& str2);
bool operator==(const char& str1,EString& str2);
bool operator!=(EString& str1, EString& str2);
bool operator!=(const wchar_t& str1, EString& str2);
bool operator!=(const char& str1, EString& str2);
bool operator<(EString& str1, EString& str2);
bool operator<(const wchar_t* str1, EString& str2);
bool operator<(const char* str1, EString& str2);
bool operator>(const wchar_t* str1, EString& str2);
bool operator>(const char* str1, EString& str2);
bool operator<=(const wchar_t* str1, EString& str2);
bool operator<=(const char* str1, EString& str2);
bool operator>=(const wchar_t* str1, EString& str2);
bool operator>=(const char* str1, EString& str2);



