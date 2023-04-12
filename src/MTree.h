#pragma once
#ifndef __MUZI_MTREE_H__
#define __MUZI_MTREE_H__
#include<concepts>
#include"MAllocator.h"
#include<array>
namespace MUZI
{
	template<typename T>
	concept __Tree_Node_Inline_Ele_Type__ = requires(T x)
	{
		std::totally_ordered<T>; // 可比较的
	};

	template<__Tree_Node_Inline_Ele_Type__ T>
	struct __MTreeNode__
	{
	public:
		const size_t array_size = 4;
		static MAllocator* alloc;
	public:
		__MTreeNode__()
		{
			for (__MTreeNode__& ele : this->node)
			{
				ele = nullptr;
			}
		}
		~__MTreeNode__()
		{
			for (__MTreeNode__& ele : this->node)
			{
				if (ele != nullptr)
				{
					this->alloc->deallocate(ele);
				}
			}
		}
	public:
		void setElement(T ele)
		{
			this->ele = ele;
		}
		int addNode()
		{
			if (this->node.size == array_size)
			{
				return -1;
			}
			
			for (auto& ptr : this->node)
			{
				if (ptr != nullptr)
				{
					ptr = this->alloc->allocate(1);
					break;
				}
			}
			return this->node.size;
		}
	public:
		T ele;
		std::array<__MTreeNode__*, array_size> node;
	};
	
	template<__Tree_Node_Inline_Ele_Type__ T>
	MAllocator* __MTreeNode__<T>::alloc = MBitmapAllocate<T>::getMAllocator();

	template<__Tree_Node_Inline_Ele_Type__ T>
	class MTree
	{
	public:
		MTree() = delete;
		~MTree() = delete;
		virtual void insert(const T&) = 0;
		virtual void earse(const T&) = 0;
		virtual bool find(const T&) = 0;
		virtual bool set(const T&, const T&) = 0;
		virtual T get() = 0;
		virtual uint64_t size() = 0;
	};

	template<__Tree_Node_Inline_Ele_Type__ T>
	class MAVLTree: public MTree
	{
	public:
		void insert(const T&) override
		{

		}
		void earse(const T&) override
		{

		}
		bool find(const T&) override
		{

		}
		bool set(const T&, const T&) override
		{

		}
		T get() override
		{

		}
	private:
		__MTreeNode__* root;
	};

	template<__Tree_Node_Inline_Ele_Type__ T>
	class MRBTree: public MTree
	{
	private:
		static MAllocator* alloc;
	public:
		MRBTree():root(nullptr),size(0){}
		~MRBTree()
		{
			this->alloc->deallocate(this->root);
		}
	public:
		void insert(const T& ele) override
		{
			if (root == nullptr)
			{
				this->root = this->alloc->allocate(1);
				this->root->ele = ele;
			}

			this->size += 1;
		}
		void earse(const T&) override
		{
			if (root == nullptr)
			{
				return;
			}

			this->size -= 1;
		}
		bool find(const T& ele) override
		{
			return (this->__findNode__(ele) != nullptr);
		}
		bool set(const T& ele, const T& o_ele) override
		{
			if (this->__findNode__(ele) == nullptr)
			{
				return false;
			}
			this->__findNode__(ele)->ele = o_ele;
		}
		T get() override
		{

		}
		uint64_t size() override
		{
			return this->size;
		}
	private:
		__MTreeNode__<T>* __createNode__()
		{
			return this->alloc->allocate(1);
		}
		__MTreeNode__<T>* __findNode__(const T& ele)
		{
			return nullptr;
		}
	private:
		__MTreeNode__<T>* root;
		uint64_t size;
	};



	template<__Tree_Node_Inline_Ele_Type__ T>
	MAllocator* MRBTree<T>::alloc = MBitmapAllocate<T>::getMAllocator();
}



#endif // __MUZI_MTREE_H__