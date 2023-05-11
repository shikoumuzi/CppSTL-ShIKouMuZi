#ifndef __MUZI_MRBTREE_H__
#define __MUZI_MRBTREE_H__
#include"MTree.h"
#include<queue>
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
				return that->color == __MRBTREE_NODE_COLOR_RED__;
			}
			inline bool isRed()
			{
				return this->color == __MRBTREE_NODE_COLOR_RED__;
			}
		};

	public:
		MRBTree() :root(nullptr), node_size(0) {}
		MRBTree(const MRBTree<T>&) = delete;
		MRBTree(MRBTree<T>&& that) noexcept
		{
			if (this->root != nullptr)
			{
				this->alloc->deallocate(this->root, 0);
			}
			this->root = that.root;
			this->node_size = that.node_size;
			that.root = nullptr;
			that.node_size = 0;
		}
		~MRBTree()
		{
			if (this->root == nullptr)
			{
				return;
			}
			std::queue<__MRBTreeNode__<T>*> node_queue;
			node_queue.push(this->root);

			while (node_queue.size != 0)
			{
				if (node_queue.front()->getChildNode(__CHILDE_NODE__::LEFT) != nullptr)
				{
					node_queue.push(node_queue.front()->getChildNode(__CHILDE_NODE__::LEFT));
				}
				if (node_queue.front()->getChildNode(__CHILDE_NODE__::RIGHT) != nullptr)
				{
					node_queue.push(node_queue.front()->getChildNode(__CHILDE_NODE__::RIGHT));
				}
				__MRBTreeNode__<T>::deleteNode(node_queue.front());
				node_queue.pop();
			}

			__MRBTreeNode__<T>::deleteNode(this->root);
		}
	public:
		void insert(const T& ele)
		{
			if (root == nullptr)
			{
				this->root = this->__createNode__();
				this->root->ele = ele;
				this->root->parent = nullptr;
			}
			this->node_size += 1;
		}
		void erase(const T& ele)
		{
			if (root == nullptr || this->__eraseNode__(ele) == nullptr)
			{
				return;
			}

			this->node_size -= 1;
		}
		bool find(const T& ele)
		{
			if (root == nullptr)
			{
				return false;
			}
			return (this->__findNode__(ele) != nullptr);
		}
		bool set(const T& ele, const T& o_ele)
		{
			if{ele != o_ele}
			{
				return false;
			}
			return this->__setNode__(ele, o_ele) != nullptr;
		}
		//T get()
		//{

		//}
		uint64_t size()
		{
			return this->node_size;
		}

	private:
		__MRBTreeNode__<T>* __createNode__()
		{
			__MRBTreeNode__<T>* ret_ptr = static_cast<__MRBTreeNode__<T>*>(__MRBTreeNode__<T>::allocNode());
			ret_ptr = new(ret_ptr) __MRBTreeNode__<T>();
			return ret_ptr;
		}
		__MRBTreeNode__<T>* __findNode__(const T& ele)
		{
			__MRBTreeNode__<T>* x = this->root;
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
		__MRBTreeNode__<T>* __insertNode__(const T& ele)
		{
			__MRBTreeNode__<T>* x = this->root, *last_x = nullptr, *root_x = this->root;
			T x_ele;
			int child = -1;
			while (x)// 寻找到插入点的parent
			{
				x_ele = x->getElement();
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
				// 如果没有找到 x 会等于 nullptr
				root_x = last_x;
			}
			__MRBTreeNode__<T>* node = (child == __CHILDE_NODE__::RIGHT) ? last_x->setChildNode(__CHILDE_NODE__::RIGHT) : node = last_x->setChildNode(__CHILDE_NODE__::LEFT);
			node->color = __MRBTREE_NODE_COLOR_RED__;
			node->setElement(ele);

			x = node;
			while (x)
			{
				if (x->parent->getChildNode(__CHILDE_NODE__::LEFT) == x)
				{
					x->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->__fixAfterInsert__(x));
				}
				else
				{
					x->parent->changeChildNode(__CHILDE_NODE__::RIGHT, this->__fixAfterInsert__(x));
				}
				x = x->parent;
			}
			return node;
		}
		__MRBTreeNode__<T>* __eraseNode__(const T& ele)
		{
			__MRBTreeNode__<T>* node = this->find(ele);
			int flag = -1;
			if (node == nullptr)
			{
				return nullptr;
			}
			// 删除
			// 第一种情况，删除的节点没有子节点，直接删除，对应3，4节点的删除叶子
			if (node->getChildNode(__CHILDE_NODE__::LEFT) == nullptr && node->getChildNode(__CHILDE_NODE__::RIGHT) == nullptr && __MRBTreeNode__<T>::isRed(node))
			{
				// 如果是父节点的左节点
				if (node == node->parent->getChildNode(__CHILDE_NODE__::LEFT))
				{
					node->parent->changeChildNode(__CHILDE_NODE__::LEFT, nullptr);
				}
				else// 如果是父节点的右边节点
				{
					node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, nullptr);
				}
			}
			// 第二种情况，删除的节点只有一个子节点, 用子节点代替，对应3节点删除中心
			else if (((flag = __CHILDE_NODE__::LEFT, node->getChildNode(__CHILDE_NODE__::LEFT)) != nullptr && node->getChildNode(__CHILDE_NODE__::RIGHT) == nullptr)
				|| ((flag = __CHILDE_NODE__::RIGHT, node->getChildNode(__CHILDE_NODE__::RIGHT)) != nullptr && node->getChildNode(__CHILDE_NODE__::LEFT) == nullptr))
			{
				// 如果是父节点的左节点
				if (node == node->parent->getChildNode(__CHILDE_NODE__::LEFT))
				{
					node->parent->changeChildNode(__CHILDE_NODE__::LEFT, node->getChildNode(flag));
				}
				else// 如果是父节点的右边节点
				{
					node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, node->getChildNode(flag));
				}
				node->getChildNode(flag)->parent = node->parent;
				node->color = __MRBTREE_NODE_COLOR_BLACK__;
			}
			// 第三种情况，删除的节点有两个子节点, 先找前驱或者后驱节点替换
			else
			{
				// 找前驱节点
				__MRBTreeNode__<T>* predecessor_node = this->__findPredecessorNode__(node);
				// 互换值，相当于互换两个节点
				T tmp_ele = predecessor_node->getElement();
				predecessor_node->setElement(node->getElement());
				node->setElement(ele);
				
				// 处理 存在一个子节点的 前驱节点
				__MRBTreeNode__<T>* replacement 
					= predecessor_node->getChildNode(__CHILDE_NODE__::LEFT) != nullptr 
					? predecessor_node->getChildNode(__CHILDE_NODE__::LEFT)
					: predecessor_node->getChildNode(__CHILDE_NODE__::RIGHT);

				// 替代节点存在一个节点
				if (replacement != nullptr)
				{
					replacement->parent = predecessor_node->parent;
					if (replacement->parent == nullptr)
					{
						// 如果是根节点
						this->root = replacement;
					}
					else// 如果这个replacement存在
					{
						if (predecessor_node == predecessor_node->parent->getChildNode(__CHILDE_NODE__::LEFT))
						{
							predecessor_node->parent->changeChildNode(__CHILDE_NODE__::LEFT, replacement);
						}
						else// 如果是父节点的右边节点
						{
							predecessor_node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, replacement);
						}
						// 释放前驱节点
						predecessor_node->changeChildNode(__CHILDE_NODE__::LEFT, nullptr);
						predecessor_node->changeChildNode(__CHILDE_NODE__::RIGHT, nullptr);
						predecessor_node->parent = nullptr;
						// 赋值 是得后续可以被释放
						node = predecessor_node;
						// 调整
						if (node->color == __MRBTREE_NODE_COLOR_BLACK__)
						{
							this->__fixAfterEarse__(replacement);
						}
					}
				}
				// 删除节点为根节点
				else if (predecessor_node->parent == nullptr)
				{
					this->root = nullptr;
				}
				// 替换节点为叶子节点，replacement为nullptr
				else
				{
					// 调整
					if (predecessor_node->color == __MRBTREE_NODE_COLOR_BLACK__)
					{
						this->__fixAfterEarse__(predecessor_node);
					}

					// 然后删除
					if (predecessor_node == predecessor_node->parent->getChildNode(__CHILDE_NODE__::LEFT))
					{
						predecessor_node->parent->changeChildNode(__CHILDE_NODE__::LEFT, nullptr);
					}
					else// 如果是父节点的右边节点
					{
						predecessor_node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, nullptr);
					}
					predecessor_node->parent = nullptr;
					node = predecessor_node;
				}


			}

			// 归还被删除的节点
			node->parent = nullptr;
			node->changeChildNode(__CHILDE_NODE__::LEFT, nullptr);
			node->changeChildNode(__CHILDE_NODE__::RIGHT, nullptr);
			__MRBTreeNode__<T>::deleteNode(node);
			return nullptr;
		}
		__MRBTreeNode__<T>* __setNode__(const T& ele, const T& o_ele)
		{
			__MRBTreeNode__<T>* ret_ptr = this->find(ele);
			if (ret_ptr != nullptr)
			{
				ret_ptr->setElement(ele);
			}
			
			return ret_ptr;
		}
		__MRBTreeNode__<T>* __fixAfterInsert__(__MRBTreeNode__<T>* node)
		{
			bool right_color = __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::RIGHT));
			bool left_color = __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT));
			bool left_left_color = __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)
				->getChildNode(__CHILDE_NODE__::LEFT));

			if(__MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::RIGHT)) && __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)))
			{
				node = this->rotateLeft(node);
			}
			else if (__MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)) && __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)
				->getChildNode(__CHILDE_NODE__::LEFT));)
			{
				node = this->rotateRight(node);
			}
			else if (__MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::RIGHT)) && __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)))
			{
				this->filpColor(node);
			}

			return node;
		}
		__MRBTreeNode__<T>* __fixAfterEarse__(__MRBTreeNode__<T>* node)
		{
			// 黑色,不能为root 因为需要parent
			if (node != root && !__MRBTreeNode__<T>::isRed(node))
			{
				// node 是左孩子
				if (node == node->parent->getChildNode(__CHILDE_NODE__::LEFT))
				{
					// 兄弟节点
					__MRBTreeNode__<T>* rnode = node->parent->getChildNode(__CHILDE_NODE__::RIGHT);

					// 判断兄弟节点是否是真正的兄弟节点(不为黑色)（基于2-3-4树）
					if (__MRBTreeNode__<T>::isRed(rnode))
					{
						// 如果不是需要左旋节点，使得位置在同一层
						rnode->color = __MRBTREE_NODE_COLOR_BLACK__;
						rnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						// 左旋
						node->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->rotateLeft(node));
						//重新回到右孩子
						rnode = node->getChildNode(__CHILDE_NODE__::RIGHT);
					}


					// // 找兄弟要，兄弟为2节点
					if (!__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::LEFT)) 
						&& !__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::RIGHT)))
					{

					}
					// 找兄弟要，兄弟为3/4节点
					else
					{
						//三节点
						if (!__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::RIGHT)))
						{
							rnode->getChildNode(__CHILDE_NODE__::LEFT)->color = __MRBTREE_NODE_COLOR_BLACK__;
							rnode->color = __MRBTREE_NODE_COLOR_RED__;
							rnode->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->rotateRight(rnode));
							rnode = 
						}
						// 四节点
						else
						{

						}
					}
				}
				// node是右孩子
				else
				{

				}



			}
			// 如果是红色的则直接染黑即可
			node->color = __MRBTREE_NODE_COLOR_BLACK__;
			return nullptr;
		}
		// 寻找前驱节点，小于node的最大值
		__MRBTreeNode__<T>* __findPredecessorNode__(__MRBTreeNode__<T>* node)
		{
			__MRBTreeNode__<T>* predecessor_node = nullptr;
			if ((predecessor_node = node->getChildNode(__CHILDE_NODE__::LEFT)) != nullptr)
			{
				// 寻找左子树中的最大点
				while (predecessor_node)
				{
					predecessor_node = predecessor_node->getChildNode(__CHILDE_NODE__::RIGHT);
				}
			}
			else
			{
				predecessor_node = node;
				//寻找父节点中的第一个 子节点作为父节点的右节点存在的人
				while (predecessor_node == node->parent->getChildNode(__CHILDE_NODE__::RIGHT))
				{
					predecessor_node = predecessor_node->parent;
				}
				predecessor_node = predecessor_node->parent;
			}
			return predecessor_node;
		}
	private:

		__MRBTreeNode__<T>* rotateLeft(__MRBTreeNode__<T>* node)
		{
			__MRBTreeNode__<T>* right_child = node->getChildNode(__CHILDE_NODE__::RIGHT);
			__MRBTreeNode__<T>* right_left_child = right_child->getChildNode(__CHILDE_NODE__::LEFT);
			right_child->changeChildNode(__CHILDE_NODE__::LEFT, node);
			node->changeChildNode(__CHILDE_NODE__::RIGHT, right_left_child);
			right_child->parent = node->parent;
			node->parent = right_child;
			right_left_child->parent = node;
			return right_child;
		}
		__MRBTreeNode__<T>* rotateRight(__MRBTreeNode__<T>* node)
		{
			__MRBTreeNode__<T>* left_child = node->getChildNode(__CHILDE_NODE__::LEFT);
			__MRBTreeNode__<T>* left_right_child = left_child->getChildNode(__CHILDE_NODE__::RIGHT);
			node->changeChildNode(__CHILDE_NODE__::LEFT, left_right_child);
			left_child->changeChildNode(__CHILDE_NODE__::RIGHT, node);
			left_child->parent = node->parent;
			left_right_child->parent = node;
			node->parent = left_child;
			return left_child;
		}
		void filpColor(__MRBTreeNode__<T>* node)
		{
			node->color = __MRBTREE_NODE_COLOR_RED__;
			node->getChildNode(__CHILDE_NODE__::RIGHT)->color = __MRBTREE_NODE_COLOR_BLACK__;
			node->getChildNode(__CHILDE_NODE__::LEFT)->color = __MRBTREE_NODE_COLOR_BLACK__;
		}
	private:
		__MRBTreeNode__<T>* root;
		uint64_t node_size;    
	};
	
	//// 未知bug 当该方法写进MRBTree时，或报错 未满足关联约束 的错误
	//template<__Tree_Node_Inline_Ele_Type__ T>
	//static MTree<T, MRBTree<T>>* getMRBTree2MTree()
	//{
	//	return new MTree< T, MRBTree<T>>;
	//}
};

#endif // !__MUZI_MRBTREE_H__
