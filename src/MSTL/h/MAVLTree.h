#pragma once

#ifndef __MUZI_MAVLTREE_H__
#define __MUZI_MAVLTREE_H__
#include"MTree.h"

namespace MUZI
{
	// AVLTree
	template<__Tree_Node_Inline_Ele_Type__ T>
	class MAVLTree
	{
	public:
		enum __CHILE_NODE__
		{
			RIGHT = 0,
			LEFT
		};
	public:
		template<__Tree_Node_Inline_Ele_Type__ T>
		struct __MAVLTreeNode__:public __MTreeNode__<T>
		{
			
		};

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
		__MAVLTreeNode__<T>* __createNode__()
		{
			__MAVLTreeNode__<T>* ret = static_cast<__MAVLTreeNode__<T>*>(__MAVLTreeNode__<T>::allocate());
			ret = new(ret) __MAVLTreeNode__<T>();
			return ret;
		}
		__MAVLTreeNode__<T>* __findNode__(const T& ele)
		{
			__MAVLTreeNode__<T>* x = this->root;
			T x_ele;
			while (x)
			{
				x_ele = x->getElement();
				if (ele == x_ele)
				{
					return x;
				}
				else if (ele > x_ele)
				{
					x = x->getChildNode(__CHILDE_NODE__::RIGHT);
				}
				else  // (ele < x_ele)
				{
					x = x->getChildNode(__CHILDE_NODE__::LEFT);
				}
			}
			return x;
		}
		__MAVLTreeNode__<T>* __insertNode__(const T& ele)
		{
			
			return nullptr;
		}
		__MAVLTreeNode__<T>* __eraseNode__(const T& ele)
		{

			return nullptr;
		}
		__MAVLTreeNode__<T>* __setNode__(const T& ele, const T& o_ele)
		{
			return nullptr;
		}
		__MAVLTreeNode__<T>* __fixAfterInsert__(__MAVLTreeNode__<T>* node)
		{
			return nullptr;
		}
		__MAVLTreeNode__<T>* __fixAfterEarse__(__MAVLTreeNode__<T>* node)
		{
		
			return nullptr;
		}
	private:
		__MAVLTreeNode__<T>* rotateLeft(__MAVLTreeNode__<T>* node)
		{
			return nullptr;
		}
		__MAVLTreeNode__<T>* rotateRight(__MAVLTreeNode__<T>* node)
		{
			return nullptr;
		}
	private:
		int deepth()
		{

		}

		void __delete__()
		{

		}
	private:
		__MAVLTreeNode__<T>* root;
		uint64_t node_count;

	};

}


#endif // !__MUZI_MAVLTREE_H__

