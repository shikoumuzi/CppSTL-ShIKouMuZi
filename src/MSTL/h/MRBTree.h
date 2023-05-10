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
			while (x)// Ѱ�ҵ�������parent
			{
				x_ele = x->getElement();
				last_x = x;
				if (ele >= x_ele)// �����ڵ��ڵ�ѡ���ڽڵ��ұ߲���
				{
					child = __CHILDE_NODE__::RIGHT;
					x = x->getChildNode(__CHILDE_NODE__::RIGHT);
				}
				else  // (ele < x_ele)
				{
					child = __CHILDE_NODE__::LEFT;
					x = x->getChildNode(__CHILDE_NODE__::LEFT);
				}
				// ���û���ҵ� x ����� nullptr
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
			// ɾ��
			// ��һ�������ɾ���Ľڵ�û���ӽڵ㣬ֱ��ɾ������Ӧ3��4�ڵ��ɾ��Ҷ��
			if (node->getChildNode(__CHILDE_NODE__::LEFT) == nullptr && node->getChildNode(__CHILDE_NODE__::RIGHT) == nullptr && __MRBTreeNode__<T>::isRed(node))
			{
				// ����Ǹ��ڵ����ڵ�
				if (node == node->parent->getChildNode(__CHILDE_NODE__::LEFT))
				{
					node->parent->changeChildNode(__CHILDE_NODE__::LEFT, nullptr);
				}
				else// ����Ǹ��ڵ���ұ߽ڵ�
				{
					node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, nullptr);
				}
			}
			// �ڶ��������ɾ���Ľڵ�ֻ��һ���ӽڵ�, ���ӽڵ���棬��Ӧ3�ڵ�ɾ������
			else if (((flag = __CHILDE_NODE__::LEFT, node->getChildNode(__CHILDE_NODE__::LEFT)) != nullptr && node->getChildNode(__CHILDE_NODE__::RIGHT) == nullptr)
				|| ((flag = __CHILDE_NODE__::RIGHT, node->getChildNode(__CHILDE_NODE__::RIGHT)) != nullptr && node->getChildNode(__CHILDE_NODE__::LEFT) == nullptr))
			{
				// ����Ǹ��ڵ����ڵ�
				if (node == node->parent->getChildNode(__CHILDE_NODE__::LEFT))
				{
					node->parent->changeChildNode(__CHILDE_NODE__::LEFT, node->getChildNode(flag));
				}
				else// ����Ǹ��ڵ���ұ߽ڵ�
				{
					node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, node->getChildNode(flag));
				}
				node->getChildNode(flag)->parent = node->parent;
				node->color = __MRBTREE_NODE_COLOR_BLACK__;
			}
			// �����������ɾ���Ľڵ��������ӽڵ�, ����ǰ�����ߺ����ڵ��滻
			else
			{
				// ��ǰ���ڵ�
				__MRBTreeNode__<T>* predecessor_node = this->__findPredecessorNode__(node);
				// ����ֵ���൱�ڻ��������ڵ�
				T tmp_ele = predecessor_node->getElement();
				predecessor_node->setElement(node->getElement());
				node->setElement(ele);
				
				// ���� ����һ���ӽڵ�� ǰ���ڵ�
				__MRBTreeNode__<T>* replacement 
					= predecessor_node->getChildNode(__CHILDE_NODE__::LEFT) != nullptr 
					? predecessor_node->getChildNode(__CHILDE_NODE__::LEFT)
					: predecessor_node->getChildNode(__CHILDE_NODE__::RIGHT);

				// ����ڵ����һ���ڵ�
				if (replacement != nullptr)
				{
					replacement->parent = predecessor_node->parent;
					if (replacement->parent == nullptr)
					{
						// ����Ǹ��ڵ�
						this->root = replacement;
					}
					else// ������replacement����
					{
						if (predecessor_node == predecessor_node->parent->getChildNode(__CHILDE_NODE__::LEFT))
						{
							predecessor_node->parent->changeChildNode(__CHILDE_NODE__::LEFT, replacement);
						}
						else// ����Ǹ��ڵ���ұ߽ڵ�
						{
							predecessor_node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, replacement);
						}
						// �ͷ�ǰ���ڵ�
						predecessor_node->changeChildNode(__CHILDE_NODE__::LEFT, nullptr);
						predecessor_node->changeChildNode(__CHILDE_NODE__::RIGHT, nullptr);
						predecessor_node->parent = nullptr;
						// ��ֵ �ǵú������Ա��ͷ�
						node = predecessor_node;
						// ����
						if (node->color == __MRBTREE_NODE_COLOR_BLACK__)
						{
							this->__fixAfterEarse__(replacement);
						}
					}
				}
				// ɾ���ڵ�Ϊ���ڵ�
				else if (predecessor_node->parent == nullptr)
				{
					this->root = nullptr;
				}
				// �滻�ڵ�ΪҶ�ӽڵ㣬replacementΪnullptr
				else
				{
					// ����
					if (predecessor_node->color == __MRBTREE_NODE_COLOR_BLACK__)
					{
						this->__fixAfterEarse__(predecessor_node);
					}

					// Ȼ��ɾ��
					if (predecessor_node == predecessor_node->parent->getChildNode(__CHILDE_NODE__::LEFT))
					{
						predecessor_node->parent->changeChildNode(__CHILDE_NODE__::LEFT, nullptr);
					}
					else// ����Ǹ��ڵ���ұ߽ڵ�
					{
						predecessor_node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, nullptr);
					}
					predecessor_node->parent = nullptr;
					node = predecessor_node;
				}


			}

			// �黹��ɾ���Ľڵ�
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
			// ��ɫ,����Ϊroot ��Ϊ��Ҫparent
			if (node != root && !__MRBTreeNode__<T>::isRed(node))
			{
				// node ������
				if (node == node->parent->getChildNode(__CHILDE_NODE__::LEFT))
				{
					// �ֵܽڵ�
					__MRBTreeNode__<T>* rnode = node->parent->getChildNode(__CHILDE_NODE__::RIGHT);

					// �ж��ֵܽڵ��Ƿ����������ֵܽڵ�(��Ϊ��ɫ)������2-3-4����
					if (__MRBTreeNode__<T>::isRed(rnode))
					{
						// ���������Ҫ�����ڵ㣬ʹ��λ����ͬһ��
						rnode->color = __MRBTREE_NODE_COLOR_BLACK__;
						rnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						// ����
						node->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->rotateLeft(node));
						//���»ص��Һ���
						rnode = node->getChildNode(__CHILDE_NODE__::RIGHT);
					}


					// // ���ֵ�Ҫ���ֵ�Ϊ2�ڵ�
					if (!__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::LEFT)) 
						&& !__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::RIGHT)))
					{

					}
					// ���ֵ�Ҫ���ֵ�Ϊ3/4�ڵ�
					else
					{
						//���ڵ�
						if (rnode->getChildNode(__CHILDE_NODE__::LEFT) != nullptr && rnode->getChildNode(__CHILDE_NODE__::RIGHT) != nullptr)
						{

						}
						// �Ľڵ�
						else
						{

						}
					}
				}
				// node���Һ���
				else
				{

				}



			}
			// ����Ǻ�ɫ����ֱ��Ⱦ�ڼ���
			node->color = __MRBTREE_NODE_COLOR_BLACK__;
			return nullptr;
		}
		// Ѱ��ǰ���ڵ㣬С��node�����ֵ
		__MRBTreeNode__<T>* __findPredecessorNode__(__MRBTreeNode__<T>* node)
		{
			__MRBTreeNode__<T>* predecessor_node = nullptr;
			if ((predecessor_node = node->getChildNode(__CHILDE_NODE__::LEFT)) != nullptr)
			{
				// Ѱ���������е�����
				while (predecessor_node)
				{
					predecessor_node = predecessor_node->getChildNode(__CHILDE_NODE__::RIGHT);
				}
			}
			else
			{
				predecessor_node = node;
				//Ѱ�Ҹ��ڵ��еĵ�һ�� �ӽڵ���Ϊ���ڵ���ҽڵ���ڵ���
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
			return right_child;
		}
		__MRBTreeNode__<T>* rotateRight(__MRBTreeNode__<T>* node)
		{
			__MRBTreeNode__<T>* left_child = node->getChildNode(__CHILDE_NODE__::LEFT);
			__MRBTreeNode__<T>* left_right_child = left_child->getChildNode(__CHILDE_NODE__::RIGHT);
			node->changeChildNode(__CHILDE_NODE__::LEFT, left_right_child);
			left_child->changeChildNode(__CHILDE_NODE__::RIGHT, node);
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
	
	// δ֪bug ���÷���д��MRBTreeʱ���򱨴� δ�������Լ�� �Ĵ���
	template<__Tree_Node_Inline_Ele_Type__ T>
	static MTree<T, MRBTree<T>>* getMRBTree2MTree()
	{
		return new MTree< T, MRBTree<T>>;
	}
};

#endif // !__MUZI_MRBTREE_H__
