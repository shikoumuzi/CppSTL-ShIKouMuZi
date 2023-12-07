#ifndef __MUZI_MRBTREE_H__
#define __MUZI_MRBTREE_H__
#include"MTree.h"
#include"MIterator.h"
#include<queue>
#include"MBase/MObjectBase.h"
namespace MUZI
{
	//RBTree
	template<__Tree_Node_Inline_Ele_Type__ T = __MDefaultTypeDefine__>
	class MRBTree
	{
	private: // childnode
		enum __CHILDE_NODE__
		{
			RIGHT = 0,
			LEFT
		};
	public:// child node
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
	public:// 迭代器
		template<__Tree_Node_Inline_Ele_Type__ T = __MDefaultTypeDefine__>
		class MIterator
		{
		public:
			friend class MRBTree<T>;
			//friend class MReverseIterator<T>;
		public:
			MIterator() :m_data(nullptr), m_parent(nullptr) {}
			MIterator(__MRBTreeNode__<T>& node, MRBTree<T>* parent) :m_data(&node), m_parent(parent) {}
			MIterator(__MRBTreeNode__<T>* node, MRBTree<T>* parent) :m_data(node), m_parent(parent) {}
			MIterator(T& ele, MRBTree<T>* parent) :m_parent(parent)
			{
				if ((this->m_data = parent->__findNode__(ele)) == nullptr)
				{
					this->m_parent = nullptr;
				}
			}
			MIterator(const MIterator<T>& it) : m_data(it.m_data), m_parent(it.m_parent) {}
			MIterator(MIterator<T>&& it) :m_data(it.m_data), m_parent(it.m_parent)
			{
				it.m_data = nullptr;
				it.m_parent = nullptr;
			}
			~MIterator()
			{
				this->m_data = nullptr;
				this->m_parent = nullptr;
			}
		public:
			void disable()
			{
				this->m_parent = nullptr;
				this->m_data = nullptr;
			}
			inline bool status()
			{
				return this->m_data != nullptr;
			}
		public:
			void operator++()
			{
				if (this->m_data == nullptr)
				{
					return;
				}
				*this += 1;
			}
			void operator--()
			{
				if (this->m_data == nullptr)
				{
					return;
				}
				*this -= 1;
			}
			std::strong_ordering operator<=>(const MIterator<T>& that)
			{
				if (this->m_data == nullptr)
				{
					return std::strong_ordering::equivalent;
				}
				if (that.m_data == nullptr)
				{
					return std::strong_ordering::less;
				}
				if (this->m_parent != that.m_parent) return std::strong_ordering::less;// 不是一个就永远小于
				// 从设计上迭代器将从最小的节点开始，有序得返回下一个大小的内容，故直接按照ele元素进行排序
				if (this->m_data->ele > that.m_data->ele) return std::strong_ordering::greater;
				if (this->m_data->ele < that.m_data->ele) return std::strong_ordering::less;
				return std::strong_ordering::equivalent;
			}
			bool operator==(MIterator<T>& it)
			{
				if (this->m_parent != it.m_parent)
				{
					return false;
				}
				if (this->m_data == nullptr)
				{
					if (it.m_data == nullptr)
					{
						return true;
					}
					return false;
				}
				return (*this <=> it) == 0;
			}
			const T operator* ()
			{
				if (this->m_data == nullptr)
				{
					return T();
				}
				return this->m_data->ele;
			}
			void operator+=(size_t step)
			{//整个点从最小值往最大值走
				if (this->m_data == nullptr)
				{
					return;
				}
				while (step--)
				{
					//如果有右节点，优先找第一个
					if (this->m_data->getChildNode(__CHILDE_NODE__::RIGHT) != nullptr)
					{
						this->m_data = this->m_data->getChildNode(__CHILDE_NODE__::RIGHT);
						while (this->m_data->getChildNode(__CHILDE_NODE__::LEFT) != nullptr)
						{
							this->m_data = this->m_data->getChildNode(__CHILDE_NODE__::LEFT);
						}
					}
					else if (this->m_data->parent != nullptr && this->m_data == this->m_data->parent->getChildNode(__CHILDE_NODE__::LEFT))
					{
						this->m_data = this->m_data->parent;
					}
					// 叶子节点，两个都为空
					else if (this->m_data->getChildNode(__CHILDE_NODE__::LEFT) == nullptr)
					{
						//右叶子
						while (this->m_data != this->m_data->parent->getChildNode(__CHILDE_NODE__::LEFT))
						{
							this->m_data = this->m_data->parent;
						}
						this->m_data = this->m_data->parent;
					}
					if (this->m_data == nullptr)
					{
						break;
					}
				}
			}
			void operator-=(size_t step)
			{
				if (this->m_data == nullptr)
				{
					return;
				}
				while (step--)
				{
					//如果有右节点，优先找第一个
					if (this->m_data->getChildNode(__CHILDE_NODE__::LEFT) != nullptr)
					{
						this->m_data = this->m_data->getChildNode(__CHILDE_NODE__::LEFT);
					}
					else if (this->m_data == this->m_data->parent->getChildNode(__CHILDE_NODE__::RIGHT))
					{
						this->m_data = this->m_data->parent;
					}
					// 叶子节点，两个都为空
					else if (this->m_data->getChildNode(__CHILDE_NODE__::RIGHT) == nullptr)
					{
						//左叶子
						while (this->m_data != this->m_data->parent->getChildNode(__CHILDE_NODE__::RIGHT))
						{
							this->m_data = this->m_data->parent;
						}
						this->m_data = this->m_data->parent;
					}
					if (this->m_data == nullptr)
					{
						__MRBTreeNode__<T>* tmp_data = this->m_parent->root;
						while (tmp_data->getChildNode(__CHILDE_NODE__::LEFT) != nullptr)
						{
							tmp_data = tmp_data->getChildNode(__CHILDE_NODE__::LEFT);
						}
						this->m_data = tmp_data;
						break;
					}
				}
			}
			void operator=(const MIterator<T>& it)
			{
				this->m_data = it.m_data;
				this->m_status = it.m_status;
				this->m_parent = it.m_parent;
			}
			void operator=(MIterator<T>&& it)
			{
				this->m_data = it.m_data;
				this->m_parent = it.m_parent;
				it.m_data = nullptr;
				it.m_parent = nullptr;
			}
			inline const __MRBTreeNode__<T>* data()
			{
				return this->m_data;
			}
			inline const __MRBTreeNode__<T>* parent()
			{
				return this->m_parent;
			}
		private:
			__MRBTreeNode__<T>* m_data;
			MRBTree<T>* m_parent;
		};
	public:// 反向迭代器
		template<__Tree_Node_Inline_Ele_Type__ T = __MDefaultTypeDefine__>
		class MReverseIterator
		{
		public:
			friend class MRBTree<T>;
		public:
			MReverseIterator() {}
			MReverseIterator(const MReverseIterator& rit) :m_iter(rit.m_iter) {}
			MReverseIterator(__MRBTreeNode__<T>& node, MRBTree<T>* parent) :m_iter(&node, parent) {}
			MReverseIterator(__MRBTreeNode__<T>* node, MRBTree<T>* parent) :m_iter(node, parent) {}
			MReverseIterator(MIterator<T>& iter) : m_iter(iter) {}
			MReverseIterator(MReverseIterator&& rit) noexcept :m_iter(std::move(rit.m_iter))
			{
				rit.disable();
			}
			~MReverseIterator()
			{
				this->m_data = nullptr;
				this->status = __ITERATOR_STAT__::DISABEL;
				this->parent = nullptr;
			}
		public:
			void disable() noexcept
			{
				this->parent = nullptr;
				this->m_data = T();
			}
			inline bool status()
			{
				return this->m_iter.status();
			}
		public:
			void operator++()
			{
				this->m_iter.operator--();
			}
			void operator--()
			{
				this->m_iter.operator++();
			}
			std::strong_ordering operator<=>(const MIterator<T>& that)
			{
				return !this->m_iter <=> (that);
			}
			bool operator!=(MIterator<T>& it)
			{
				if (this->status == __ITERATOR_STAT__::DISABLE || this->parent != it.m_parent)
				{
					return true;
				}
				return (*this <=> it) != 0;
			}
			const T operator* ()
			{
				if (this->status == __ITERATOR_STAT__::DISABLE)
				{
					return T();
				}
				return this->m_data->ele;
			}
			void operator+=(size_t step)
			{
				this->m_iter += step;
			}
			void operator-=(size_t step)
			{
				this->m_iter += step;
				if (this->m_iter == this->m_iter.m_parent()->end())
				{
					this->m_iter -= 1;
				}
			}
			void operator=(const MReverseIterator<T>& it)
			{
				this->m_iter = it.m_iter;
			}
			void operator=(MReverseIterator<T>&& it)
			{
				this->m_iter = std::move(it);
			}
			const __MRBTreeNode__<T>* data()
			{
				return this->m_iter.data();
			}
			const __MRBTreeNode__<T>* data() const
			{
				return this->m_iter.data();
			}
			MIterator<T> base()
			{
				return this->m_iter;
			}
		private:
			MIterator<T> m_iter;
		};

		using iterator = MIterator<T>;
		using reverse_iterator = MReverseIterator<T>;
	public:
		MRBTree() :root(nullptr), node_count(0) {}
		MRBTree(const MRBTree<T>&) = delete;
		MRBTree(MRBTree<T>&& that) noexcept
		{
			if (this->root != nullptr)
			{
				this->__delete__();
			}
			this->root = that.root;
			this->node_count = that.node_count;
			that.root = nullptr;
			that.node_count = 0;
		}
		~MRBTree()
		{
			this->__delete__();
		}
	public:
		MIterator<T> begin()
		{
			__MRBTreeNode__<T>* p = this->root;
			while (p->getChildNode(__CHILDE_NODE__::LEFT) != nullptr)
			{
				p = p->getChildNode(__CHILDE_NODE__::LEFT);
			}
			return MIterator<T>(*p, this);
		}
		const MIterator<T> end() const
		{
			return this->final_it;
		}
		MReverseIterator<T> rbegin()
		{
			__MRBTreeNode__<T>* p_ret = this->root;
			while (p_ret->getChildNode(__CHILDE_NODE__::RIGHT) != nullptr)
			{
				p_ret = p_ret->getChildNode(__CHILDE_NODE__::RIGHT);
			}
			return MReverseIterator<T>(p_ret, this);
		}
		const MReverseIterator<T> rend() const
		{
			return this->final_rit;
		}
	public:
		MIterator<T> insert(MIterator<T>& it)
		{
			//if (it.parent != this)
			//{
			//	return iterator<T>();
			//}
			return MIterator<T>(this->__insertNode__(it.m_data->ele), this);
		}
		void insert(const T& ele)
		{
			if (root == nullptr)
			{
				this->root = this->__createNode__();
				this->root->ele = ele;
				this->root->parent = nullptr;
			}
			else
			{
				this->__insertNode__(ele);
			}
			this->node_count += 1;
		}
		MIterator<T> erase(MIterator<T>& it)
		{
			MIterator<T> it_tmp(it);
			it_tmp += 1;
			this->erase(it.m_data->ele);
			it.disable();
			return it_tmp;
		}
		MReverseIterator<T> earse(MReverseIterator<T>& it)
		{
			MReverseIterator<T> it_tmp(it);
			it_tmp += 1;
			this->erase(it.base().m_data->ele);
			it.disable();
			return it_tmp;
		}
		void erase(const T& ele)
		{
			if (root == nullptr || this->__eraseNode__(ele) == nullptr)
			{
				return;
			}

			this->node_count -= 1;
		}
		T* find(const T& ele) const
		{
			if (root == nullptr)
			{
				return false;
			}
			__MRBTreeNode__<T>* result;
			return ((result = this->__findNode__(ele)) != nullptr) ? &result->ele : nullptr;
		}
		bool set(const T& ele, const T& o_ele)
		{
			if (ele != o_ele)
			{
				return false;
			}
			return this->__setNode__(ele, o_ele) != nullptr;
		}
		uint64_t size()
		{
			return this->node_count;
		}
	public:
		void operator=(MRBTree<T>&& that)
		{
			this->__delete__();
			this->node_count = that.node_count;
			this->root = that.root;
			that.__delete__();
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
			__MRBTreeNode__<T>* x = this->root, * last_x = nullptr, * root_x = this->root;
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
			__MRBTreeNode__<T>* node = this->__findNode__(ele);
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
						// 调整444
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
			__MRBTreeNode__<T>* ret_ptr = this->__findNode__(ele);
			if (ret_ptr != nullptr)
			{
				ret_ptr->setElement(ele);
			}
			else
			{
				this->insert(ele);
			}
			return ret_ptr;
		}
		__MRBTreeNode__<T>* __fixAfterInsert__(__MRBTreeNode__<T>* node)
		{
			bool right_color = __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::RIGHT));
			bool left_color = __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT));
			bool left_left_color = __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)->getChildNode(__CHILDE_NODE__::LEFT));

			if (__MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::RIGHT)) && __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)))
			{
				node = this->rotateLeft(node);
			}
			else if (__MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)) && __MRBTreeNode__<T>::isRed(node->getChildNode(__CHILDE_NODE__::LEFT)
				->getChildNode(__CHILDE_NODE__::LEFT)))
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
			// 循环主要以兄弟节点无法借的情况，只能从删除节点开始，向上平衡整个红黑树
			while (node != root && !__MRBTreeNode__<T>::isRed(node))
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

					// // 找兄弟要，兄弟为2节点, 需要将兄弟节点退化为红色，同父节点一同成为3/4节点
					if (!__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::LEFT))
						&& !__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::RIGHT)))
					{
						rnode->color = __MRBTREE_NODE_COLOR_RED__;// 全部损失，因为上面已经提前确定兄弟节点了
						node = node->parent;
					}
					// 找兄弟要，兄弟为3/4节点
					else
					{
						//三节点的特殊处理
						if (!__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::RIGHT)))
						{
							rnode->getChildNode(__CHILDE_NODE__::LEFT)->color = __MRBTREE_NODE_COLOR_BLACK__;
							rnode->color = __MRBTREE_NODE_COLOR_RED__;
							// 先把三节点中较小值换上来
							rnode->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->rotateRight(rnode));
							rnode = node->parent->getChildNode(__CHILDE_NODE__::RIGHT);
						}
						rnode->color = rnode->parent->color;
						rnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						rnode->getChildNode(__CHILDE_NODE__::RIGHT)->color = __MRBTREE_NODE_COLOR_BLACK__;

						// 然后左旋两个节点的父节点，将变换后的右兄弟换上来，使得兄弟节点的红色子节点转移令node成为三节点或者四节点
						if (rnode->parent->parent == nullptr)
						{
							this->root = rotateLeft(node->parent);
						}
						else if (rnode->parent == rnode->parent->parent->getChildNode(__CHILDE_NODE__::LEFT))
						{
							rnode->parent->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->rotateLeft(node->parent));
						}
						else if (rnode->parent == rnode->parent->parent->getChildNode(__CHILDE_NODE__::RIGHT))
						{
							rnode->parent->parent->changeChildNode(__CHILDE_NODE__::RIGHT, this->rotateLeft(node->parent));
						}
						node->color = __MRBTREE_NODE_COLOR_RED__;
						// 改成root 跳出while
						node = this->root;
					}
				}
				// node是右孩子
				else
				{
					// 兄弟节点
					__MRBTreeNode__<T>* lnode = node->parent->getChildNode(__CHILDE_NODE__::LEFT);

					// 判断兄弟节点是否是真正的兄弟节点(不为黑色)（基于2-3-4树）
					if (__MRBTreeNode__<T>::isRed(lnode))
					{
						// 如果不是需要左旋节点，使得位置在同一层
						lnode->color = __MRBTREE_NODE_COLOR_BLACK__;
						lnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						// 左旋
						node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, this->rotateRight(node));
						//重新回到右孩子
						lnode = node->getChildNode(__CHILDE_NODE__::LEFT);
					}

					// // 找兄弟要，兄弟为2节点, 需要将兄弟节点退化为红色，同父节点一同成为3/4节点
					if (!__MRBTreeNode__<T>::isRed(lnode->getChildNode(__CHILDE_NODE__::RIGHT))
						&& !__MRBTreeNode__<T>::isRed(lnode->getChildNode(__CHILDE_NODE__::LEFT)))
					{
						lnode->color = __MRBTREE_NODE_COLOR_RED__;// 全部损失，因为上面已经提前确定兄弟节点了
						node = node->parent;
					}
					// 找兄弟要，兄弟为3/4节点
					else
					{
						//三节点的特殊处理
						if (!__MRBTreeNode__<T>::isRed(lnode->getChildNode(__CHILDE_NODE__::LEFT)))
						{
							lnode->getChildNode(__CHILDE_NODE__::RIGHT)->color = __MRBTREE_NODE_COLOR_BLACK__;
							lnode->color = __MRBTREE_NODE_COLOR_RED__;
							// 先把三节点中较小值换上来
							lnode->parent->changeChildNode(__CHILDE_NODE__::RIGHT, this->rotateLeft(lnode));
							lnode = node->parent->getChildNode(__CHILDE_NODE__::LEFT);
						}
						lnode->color = lnode->parent->color;
						lnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						lnode->getChildNode(__CHILDE_NODE__::LEFT)->color = __MRBTREE_NODE_COLOR_BLACK__;

						// 然后左旋两个节点的父节点，将变换后的右兄弟换上来，使得兄弟节点的红色子节点转移令node成为三节点或者四节点
						if (lnode->parent->parent == nullptr)
						{
							this->root = rotateRight(node->parent);
						}
						else if (lnode->parent == lnode->parent->parent->getChildNode(__CHILDE_NODE__::RIGHT))
						{
							lnode->parent->parent->changeChildNode(__CHILDE_NODE__::RIGHT, this->rotateRight(node->parent));
						}
						else if (lnode->parent == lnode->parent->parent->getChildNode(__CHILDE_NODE__::LEFT))
						{
							lnode->parent->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->rotateRight(node->parent));
						}
						node->color = __MRBTREE_NODE_COLOR_RED__;
						// 改成root 跳出while
						node = this->root;
					}
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
		void __delete__()
		{
			if (this->root == nullptr)
			{
				return;
			}
			std::queue<__MRBTreeNode__<T>*> node_queue;
			node_queue.push(this->root);

			while (node_queue.size() != 0)
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
		uint64_t node_count;
		MIterator<T> final_it;
	};

	// 未知bug 当该方法写进MRBTree时，或报错 未满足关联约束 的错误
	template<__Tree_Node_Inline_Ele_Type__ T>
	static MTree<T, MRBTree<T>>* MRBTree2MTree()
	{
		return new MTree< T, MRBTree<T>>;
	}
};

#endif // !__MUZI_MRBTREE_H__
