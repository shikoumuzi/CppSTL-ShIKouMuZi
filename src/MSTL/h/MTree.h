#pragma once
#ifndef __MUZI_MTREE_H__
#define __MUZI_MTREE_H__
#include<concepts>
#include"MBitmapAllocator.h"
#include<array>
#include<new>
namespace MUZI
{
	// Node
	template<typename T>
	concept __Tree_Node_Inline_Ele_Type__ = requires(T x, T y)
	{
		std::totally_ordered<T>; // �ɱȽϵ�
		{x = y};
	};

	template<__Tree_Node_Inline_Ele_Type__ T, typename Node_Type, size_t LEN = 4>
	struct __MTreeNode__
	{
	public:
		using value_type = T;
		using reference = T&;
		using const_reference = const T&;
	private:
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
		inline Node_Type* changeChildNode(int sign, Node_Type* node)
		{
			this->node[sign] = node;
		}
	public:
		static void* allocNode()
		{
			return static_cast<void*>(alloc->allocate(1));
		}
		static void deleteNode(Node_Type* node)
		{
			alloc->deallocate(node);
		}
	public:
		T ele;
		Node_Type* parent;
		std::array<Node_Type*, LEN> node;
	};
	
	template<__Tree_Node_Inline_Ele_Type__ T, typename Node_Type, size_t LEN>
	MAllocator* __MTreeNode__<T, Node_Type, LEN>::alloc = MBitmapAllocator< __MTreeNode__<T, Node_Type>>::getMAllocator();

	// Tree
	template<typename T, typename U>
	concept __MTree_Type__ = requires(T x, U u1, U u2)
	{	
		{x.insert(u1)} -> std::same_as<void>;
		{x.erase(u1)} -> std::same_as<void>;
		{x.find(u1)} -> std::convertible_to<bool>;
		{x.set(u1, u2)} -> std::convertible_to<bool>;
	/*	{x.get()} -> std::same_as<U>;*/
		{x.size()} -> std::convertible_to<uint64_t>;
	};

	template<__Tree_Node_Inline_Ele_Type__ T, __MTree_Type__<T> Tree>
	class MTree
	{
	public:
		using tree_type = Tree;
		using ele_type = T;
		using value_type = T;
		using reference = T&;
		using const_reference = const T&;
		using size_type = size_t;
	public:
		MTree(Tree tree)
		{
			this->tree = tree;
		}
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