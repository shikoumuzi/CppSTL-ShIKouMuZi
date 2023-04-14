#pragma once
#ifndef __MUZI_MTREE_H__
#define __MUZI_MTREE_H__
#include<concepts>
#include"MAllocator.h"
#include<array>
#include<new>
namespace MUZI
{
	// Node
	template<typename T>
	concept __Tree_Node_Inline_Ele_Type__ = requires(T x)
	{
		std::totally_ordered<T>; // 可比较的
		x.operator=();
	};

	template<typename T>
	concept __Tree_Node__Type__ = requires(T x)
	{
		x.parent;
		x.ele;
	};


	template<__Tree_Node_Inline_Ele_Type__ T, __Tree_Node__Type__ Node_Type, size_t LEN = 4>
	struct __MTreeNode__
	{
	public:
		static MAllocator* alloc;
	public:
		__MTreeNode__()
		{
			this->parent = nullptr;
			for (__MTreeNode__& ele : this->node)
			{
				ele = nullptr;
			}
		}
		__MTreeNode__(T ele)
		{
			this->ele = ele;
			this->parent = nullptr;
			for (__MTreeNode__& ele_ : this->node)
			{
				ele_ = nullptr;
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
		T& getElement()
		{
			return this->ele;
		}
		int addNode()
		{
			if (this->node.size == LEN)
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
		int compareTo(const Node_Type& that)
		{
			if (this->ele < that.ele) return -1;
			else if (this->ele > that.ele) return 1;
			return 0;
		}
		int compareTo(const Node_Type* that)
		{
			if (this->ele < that->ele) return -1;
			else if (this->ele > that->ele) return 1;
			return 0;
		}
		Node_Type* getChildNode(int sign)
		{
			if (sign > LEN)
			{
				return nullptr;
			}
			return this->node[sign];
		}
		Node_Type* setChildNode(int sign)
		{
			this->node[sign] = this->alloc->allocate(1);
			this->node[sign] = new(this->node[sign]) Node_Type();
			this->node[sign]->parent = this;
			return this->node[sign];
		}
		Node_Type* changeChildNode(int sign, Node_Type* node)
		{
			this->node[sign] = node;
		}
	public:
		T ele;
		Node_Type* parent;
		std::array<Node_Type*, LEN> node;
	};
	
	template<__Tree_Node_Inline_Ele_Type__ T, __Tree_Node__Type__ Node_Type, size_t LEN>
	MAllocator* __MTreeNode__<T, Node_Type, LEN>::alloc = MBitmapAllocate< __MTreeNode__<T, Node_Type>>::getMAllocator();

	// Tree
	template<typename T>
	concept __MTree_Type__ = requires(T x)
	{
		x.insert();
		x.earse();
		x.find();
		x.set();
		x.get();
		x.size();

	};

	template<__Tree_Node_Inline_Ele_Type__ T, __MTree_Type__ Tree = nullptr_t>
	class MTree
	{
	public:
		using tree_type = Tree;
		using ele_type = T;
	public:
		MTree()
		{}
		~MTree()
		{}
		inline void insert(const T& that)
		{
			this->tree.insert(that);
		}
		inline void earse(const T& that)
		{
			this->tree.earse(that);
		}
		inline bool find(const T& that)
		{
			return this->tree.find(that);
		}
		inline bool set(const T& old_this, const T& new_that)
		{
			return this->tree.set(old_this, new_that);
		}
		T get()
		{
			return this->tree.get();
		}
		uint64_t size()
		{
			return this->tree.size();
		}
	private:
		Tree tree;
	};



}



#endif // __MUZI_MTREE_H__