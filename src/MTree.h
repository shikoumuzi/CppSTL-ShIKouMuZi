#pragma once
#ifndef __MUZI_MTREE_H__
#define __MUZI_MTREE_H__
#include<concepts>
#include"MAllocator.h"
#include<array>
namespace MUZI
{
	// Node
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

	// AVLTree
	template<__Tree_Node_Inline_Ele_Type__ T>
	class MAVLTree
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

	//RBTree
	template<__Tree_Node_Inline_Ele_Type__ T>
	class MRBTree
	{
	public:
		using class_type = MRBTree;
	private:
		static MAllocator* alloc;
	public:
		static MTree<T, class_type>* getTree()
		{
			return new MTree<T, class_type>();
		}
	private:
		MRBTree() :root(nullptr), size(0) noexcept {}
		MRBTree(const MRBTree<T>&) = delete;
		MRBTree(MRBTree<T>&& that) noexcept
		{
			if (this->root != nullptr)
			{
				this->alloc->deallocate(this->root);
			}
			this->root = that.root;
			this->size = that.size;
			that.root = nullptr;
			that.size = 0;
		}
	public:
		~MRBTree()
		{
			this->alloc->deallocate(this->root);
		}
	public:
		void insert(const T& ele)
		{
			if (root == nullptr)
			{
				this->root = this->__createNode__();
				this->root->ele = ele;
			}

			this->size += 1;
		}
		void earse(const T&) 
		{
			if (root == nullptr)
			{
				return;
			}

			this->size -= 1;
		}
		inline bool find(const T& ele)
		{
			return (this->__findNode__(ele) != nullptr);
		}
		inline bool set(const T& ele, const T& o_ele)
		{
			return this->__setNode__(ele, e_ele) != nullptr;
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
		__MTreeNode__<T>* __insertNode__(const T& ele)
		{
			return nullptr;
		}
		__MTreeNode__<T>* __earseNode__(const T& ele)
		{
			return nullptr;
		}
		__MTreeNode__<T>* __setNode__(const T& ele, const T& o_ele)
		{
			__MTreeNode__<T>* p_tmp = this->__findNode__(ele);
			if (p_tmp != nullptr)
			{
				return p_tmp;
			}


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