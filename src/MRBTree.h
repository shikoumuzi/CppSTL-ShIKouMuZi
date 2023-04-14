#ifndef __MUZI_MRBTREE_H__
#define __MUZI_MRBTREE_H__
#include"MTree.h"
namespace MUZI
{
	//RBTree
	template<__Tree_Node_Inline_Ele_Type__ T>
	class MRBTree
	{
	private: // childnode
		enum __CHILDE_NODE__
		{
			RIGHT = 0,
			LEFT
		};
	private:// child node
		template<__Tree_Node_Inline_Ele_Type__ T>
		struct __MRBTreeNode__ : public __MTreeNode__<T, __MRBTreeNode__<T>, 2>
		{
		public:
#define __MRBTREE_NODE_COLOR_RED__ true
#define __MRBTREE_NODE_COLOR_BLACK__ false 
		public:
			__MRBTreeNode__() :color(__MRBTREE_NODE_COLOR_RED__) {}
			__MRBTreeNode__(T ele, bool color) :color(color)
			{
				this->ele = ele;
			}
		public:
			bool color; // red is true, black is false
		public:
			static bool isRed(const __MRBTreeNode__* that)
			{
				if (that == nullptr) return __MRBTREE_NODE_COLOR_BLACK__;
				return this->color == __MRBTREE_NODE_COLOR_RED__;
			}
			inline bool isRed()
			{
				return this->color == __MRBTREE_NODE_COLOR_RED__;
			}
		};

	public:
		using class_type = MRBTree;
	private:
		static MAllocator* alloc;

	public:
		static MTree<T, class_type>* getTree()
		{
			return new MTree<T, class_type>();
		}
	public:
		MRBTree() :root(nullptr), size(0) {}
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

			this->node_size += 1;
		}
		void earse(const T&)
		{
			if (root == nullptr)
			{
				return;
			}

			this->node_size -= 1;
		}
		inline bool find(const T& ele)
		{
			return (this->__findNode__(ele) != nullptr);
		}
		inline bool set(const T& ele, const T& o_ele)
		{
			return this->__setNode__(ele, o_ele) != nullptr;
		}
		bool set_value(const T& ele, const T& o_ele)
		{
			__MRBTreeNode__<T>* node_tmp = this->__findNode__(ele);
			if (node_tmp == nullptr)
			{
				return false;
			}
			node_tmp->ele = o_ele;
		}
		T get() override
		{

		}
		uint64_t size() override
		{
			return this->size;
		}

	private:
		__MRBTreeNode__<T>* __createNode__()
		{
			__MRBTreeNode__<T>* ret_ptr = this->alloc->allocate(1);
			ret_ptr = new(ret_ptr) __MRBTreeNode__<T>();
			return ret_ptr;
		}
		__MRBTreeNode__<T>* __findNode__(const T& ele)
		{
			if (this->root == nullptr)
			{
				return nullptr;
			}
			__MRBTreeNode__<T>* x = this->root;
			T x_ele;
			while (x)
			{
				x_ele = x->getElement();
				if (ele = x_ele)
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

			return nullptr;
		}
		__MRBTreeNode__<T>* __insertNode__(const T& ele)
		{
			__MRBTreeNode__<T>* x = this->root, last_x = nullptr, root_x = this->root;
			T x_ele;
			int child = -1;
			while (x)
			{
				x_ele = x->getEle();
				last_x = x;
				if (ele >= x_ele)// 将大于等于的选择在节点右边插入
				{
					child = __CHILDE_NODE__::RIGHT;
					x = x->getChildNode(__CHILDE_NODE__::RIGHT);
				}
				else  // (ele < x_ele)
				{
					child = __CHILDE_NODE__::LEFT;
					x = x->getChildNode(__CHILDE_NODE__::LEFT);
				}
				root_x = last_x;
			}
			__MRBTreeNode__<T>* node = (child == __CHILDE_NODE__::RIGHT) ? last_x.setChildNode(__CHILDE_NODE__::RIGHT) : node = last_x.setChildNode(__CHILDE_NODE__::LEFT);
			node->color = __MRBTREE_NODE_COLOR_RED__;
			node->setElement(ele);

			x = node;
			while (x)
			{
				if (x->parent->getChildNode(__CHILDE_NODE__::LEFT) == x)
				{
					x->parent->changeChildNode(__CHILDE_NODE__::LEFT) = this->__checkNode__(x);
				}
				else
				{
					x->parent->changeChildNode(__CHILDE_NODE__::RIGHT) = this->__checkNode__(x);
				}
				x = x->parent;
			}
			return node;
		}
		__MRBTreeNode__<T>* __earseNode__(const T& ele)
		{
			return nullptr;
		}
		__MRBTreeNode__<T>* __setNode__(const T& ele, const T& o_ele)
		{
			__MRBTreeNode__<T>* ret_ptr = nullptr;
			if ((ret_ptr = this->earse(ele)) == nullptr) return nullptr;
			else if ((ret_ptr = this->insert(o_ele)) == nullptr) return nullptr;
			return ret_ptr;
		}
		__MRBTreeNode__<T>* __checkNode__(__MRBTreeNode__<T>* node)
		{
			bool right_color = __MRBTreeNode__::isRed(node->getChildNode(__CHILDE_NODE__::RIGHT));
			bool left_color = __MRBTreeNode__::isRed(node->getChildNode(__CHILDE_NODE__::LEFT));
			bool left_left_color = __MRBTreeNode__::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)
				->getChildNode(__CHILDE_NODE__::LEFT));

			if(right_color && !left_color)
			{
				node = this->rotateLeft(node);
			}
			else if (left_color && left_left_color)
			{
				node = this->rotateRight(node);
			}
			else if (right_color && left_color)
			{
				this->filpColor(node);
			}

			return node;
		}
	private:

		__MRBTreeNode__<T>* rotateLeft(__MRBTreeNode__<T>* node)
		{
			return node;
		}
		__MRBTreeNode__<T>* rotateRight(__MRBTreeNode__<T>* node)
		{
			return node;
		}
		void filpColor(__MRBTreeNode__<T>* node)
		{
			node->color = __MRBTREE_NODE_COLOR_RED__;
			node->getChildNode(__CHILDE_NODE__::RIGHT)->color = __MRBTREE_NODE_COLOR_BLACK__;
			node->getChildNode(__CHILDE_NODE__::LEFT)->color = __MRBTREE_NODE_COLOR_BLACK__;
		}
	private:
		__MTreeNode__<T, __MRBTreeNode__<T>, 2>* root;
		uint64_t node_size;
	};
	template<__Tree_Node_Inline_Ele_Type__ T>
	MAllocator* MRBTree<T>::alloc = MBitmapAllocate<T>::getMAllocator();
};

#endif // !__MUZI_MRBTREE_H__
