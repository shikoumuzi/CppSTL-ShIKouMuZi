#pragma once
#include<iostream>
#include<stdio.h>
#include<stdlib.h>

#define EDEQUE_PRIVATE_SIZE 16

template<class T>
struct EDequePrivate
{
public:
	EDequePrivate()
	{
		this->data = new T[EDEQUE_PRIVATE_SIZE];
		this->next = NULL;
		this->parent = NULL;
	}
	EDequePrivate(T* data, EDequePrivate<T>* parent)
	{
		this->data = new T[EDEQUE_PRIVATE_SIZE];
		memset(this->data, data, EDEQUE_PRIVATE_SIZE);
		this->next = nullptr;
		this->parent = parent;
	}
	~EDequePrivate()
	{
		delete this->data;
		if (this->next != nullptr)
		{
			delete this->next;
		}
	}
public:
	T *data;
	EDequePrivate<T>* next;
	EDequePrivate<T>* parent;
};

template<class T>
class EDeque
{
public:
	class Iterator
	{
	public:
		T* ptr;
		EDequePrivate<T>* group;
		size_t index;
		EDeque<T>* parent;
	};
	using iterator = Iterator;
public:
	EDeque() :d(nullptr),d_group(nullptr)
	{
		this->length		= 0;
		this->group			= 0;
		this->totallength	= 0;
		this->startindex	= 0;
		this->endindex		= 0;
	}
	EDeque(EDeque<T>& other)
		:d(new EDequePrivate(other.d,nullptr,nullptr))
	{
		size_t count = 0;
		EDequePrivate<T>* parent = this->d;
		for (EDequePrivate<T>* it = other.d-> next, it_self = this->d->next; it != nullptr; it = it->next,it_self = it_self->next)
		{
			++count;
			it_self = new EDequePrivate<T>((it->data), parent);
			parent->next = it_self;
			parent = it_self;
		}
		this->startindex	= other.startindex;
		this->endindex		= other.endindex;
		this->group			= other.group;
		this->length		= other.length;
		this->totallength	= other.totallength;
		this->d_group		= new EDequePrivate<T>*[count];

		size_t i = 0;
		for (EDequePrivate<T>* it = this->d; it != nullptr; it = it->next)
		{
			this->d_group[i] = it;
			++i;
		}
	}
	EDeque(EDeque<T>&& other)
		:d(other.d),d_group(other.d_group)
	{
		other.d				= nullptr;
		other.d_group		= nullptr;
		this->startindex	= other.startindex;
		this->endindex		= other.endindex;
		this->group			= other.group;
		this->length		= other.length;
		this->totallength	= other.totallength;
	}
	~EDeque()
	{
		delete this->d;
	}
public:
	void operator=(EDeque<T>& other)
	{
		if (this->d == nullptr)
		{
			this->d = new EDequePrivate(other.d, nullptr, nullptr);
		}
		else
		{
			delete this->d;
			delete this->d_group;
			this->d			= new EDequePrivate<T>(other.d, nullptr, nullptr);
			this->d_group	= new EDeque<T>*[other.group];

			size_t i = 0;
			for (EDequePrivate<T>* it = this->d; it != nullptr; it = it->next)
			{
				this->d_group[i] = it;
				++i;
			}
		}
		EDequePrivate<T>* parent = this->d;
		for (EDequePrivate<T>* it = other.d->next, it_self = this->d->next; it != nullptr; it = it->next, it_self = it_self->next)
		{
			it_self = new EDequePrivate((it->data), parent);
			parent->next = it_self;
			parent = it_self;
		}
		this->startindex = other.startindex;
		this->endindex = other.endindex;
	}
	T& operator[](size_t index)
	{
		if (index >= this->length)
		{
			throw "index out of range";
			exit(1);
		}
		size_t group		= index / EDEQUE_PRIVATE_SIZE;
		size_t index_self	= index % EDEQUE_PRIVATE_SIZE + this->startindex;
		if (index_self >= EDEQUE_PRIVATE_SIZE)
		{
			++group;
			index_self %= EDEQUE_PRIVATE_SIZE;
		}
		return d_group[group]->data[index_self];
		
	}
private:
	EDequePrivate<T>* d;
	EDequePrivate<T>** d_group;
	size_t length;
	size_t totallength;
	size_t group;
private:
	size_t startindex;
	size_t endindex;
};