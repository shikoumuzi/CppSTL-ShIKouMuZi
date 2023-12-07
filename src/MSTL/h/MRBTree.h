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
	public:// ������
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
				if (this->m_parent != that.m_parent) return std::strong_ordering::less;// ����һ������ԶС��
				// ������ϵ�����������С�Ľڵ㿪ʼ������÷�����һ����С�����ݣ���ֱ�Ӱ���eleԪ�ؽ�������
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
			{//���������Сֵ�����ֵ��
				if (this->m_data == nullptr)
				{
					return;
				}
				while (step--)
				{
					//������ҽڵ㣬�����ҵ�һ��
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
					// Ҷ�ӽڵ㣬������Ϊ��
					else if (this->m_data->getChildNode(__CHILDE_NODE__::LEFT) == nullptr)
					{
						//��Ҷ��
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
					//������ҽڵ㣬�����ҵ�һ��
					if (this->m_data->getChildNode(__CHILDE_NODE__::LEFT) != nullptr)
					{
						this->m_data = this->m_data->getChildNode(__CHILDE_NODE__::LEFT);
					}
					else if (this->m_data == this->m_data->parent->getChildNode(__CHILDE_NODE__::RIGHT))
					{
						this->m_data = this->m_data->parent;
					}
					// Ҷ�ӽڵ㣬������Ϊ��
					else if (this->m_data->getChildNode(__CHILDE_NODE__::RIGHT) == nullptr)
					{
						//��Ҷ��
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
	public:// ���������
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
			__MRBTreeNode__<T>* node = this->__findNode__(ele);
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
						// ����444
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
			// ��ɫ,����Ϊroot ��Ϊ��Ҫparent
			// ѭ����Ҫ���ֵܽڵ��޷���������ֻ�ܴ�ɾ���ڵ㿪ʼ������ƽ�����������
			while (node != root && !__MRBTreeNode__<T>::isRed(node))
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

					// // ���ֵ�Ҫ���ֵ�Ϊ2�ڵ�, ��Ҫ���ֵܽڵ��˻�Ϊ��ɫ��ͬ���ڵ�һͬ��Ϊ3/4�ڵ�
					if (!__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::LEFT))
						&& !__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::RIGHT)))
					{
						rnode->color = __MRBTREE_NODE_COLOR_RED__;// ȫ����ʧ����Ϊ�����Ѿ���ǰȷ���ֵܽڵ���
						node = node->parent;
					}
					// ���ֵ�Ҫ���ֵ�Ϊ3/4�ڵ�
					else
					{
						//���ڵ�����⴦��
						if (!__MRBTreeNode__<T>::isRed(rnode->getChildNode(__CHILDE_NODE__::RIGHT)))
						{
							rnode->getChildNode(__CHILDE_NODE__::LEFT)->color = __MRBTREE_NODE_COLOR_BLACK__;
							rnode->color = __MRBTREE_NODE_COLOR_RED__;
							// �Ȱ����ڵ��н�Сֵ������
							rnode->parent->changeChildNode(__CHILDE_NODE__::LEFT, this->rotateRight(rnode));
							rnode = node->parent->getChildNode(__CHILDE_NODE__::RIGHT);
						}
						rnode->color = rnode->parent->color;
						rnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						rnode->getChildNode(__CHILDE_NODE__::RIGHT)->color = __MRBTREE_NODE_COLOR_BLACK__;

						// Ȼ�����������ڵ�ĸ��ڵ㣬���任������ֵܻ�������ʹ���ֵܽڵ�ĺ�ɫ�ӽڵ�ת����node��Ϊ���ڵ�����Ľڵ�
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
						// �ĳ�root ����while
						node = this->root;
					}
				}
				// node���Һ���
				else
				{
					// �ֵܽڵ�
					__MRBTreeNode__<T>* lnode = node->parent->getChildNode(__CHILDE_NODE__::LEFT);

					// �ж��ֵܽڵ��Ƿ����������ֵܽڵ�(��Ϊ��ɫ)������2-3-4����
					if (__MRBTreeNode__<T>::isRed(lnode))
					{
						// ���������Ҫ�����ڵ㣬ʹ��λ����ͬһ��
						lnode->color = __MRBTREE_NODE_COLOR_BLACK__;
						lnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						// ����
						node->parent->changeChildNode(__CHILDE_NODE__::RIGHT, this->rotateRight(node));
						//���»ص��Һ���
						lnode = node->getChildNode(__CHILDE_NODE__::LEFT);
					}

					// // ���ֵ�Ҫ���ֵ�Ϊ2�ڵ�, ��Ҫ���ֵܽڵ��˻�Ϊ��ɫ��ͬ���ڵ�һͬ��Ϊ3/4�ڵ�
					if (!__MRBTreeNode__<T>::isRed(lnode->getChildNode(__CHILDE_NODE__::RIGHT))
						&& !__MRBTreeNode__<T>::isRed(lnode->getChildNode(__CHILDE_NODE__::LEFT)))
					{
						lnode->color = __MRBTREE_NODE_COLOR_RED__;// ȫ����ʧ����Ϊ�����Ѿ���ǰȷ���ֵܽڵ���
						node = node->parent;
					}
					// ���ֵ�Ҫ���ֵ�Ϊ3/4�ڵ�
					else
					{
						//���ڵ�����⴦��
						if (!__MRBTreeNode__<T>::isRed(lnode->getChildNode(__CHILDE_NODE__::LEFT)))
						{
							lnode->getChildNode(__CHILDE_NODE__::RIGHT)->color = __MRBTREE_NODE_COLOR_BLACK__;
							lnode->color = __MRBTREE_NODE_COLOR_RED__;
							// �Ȱ����ڵ��н�Сֵ������
							lnode->parent->changeChildNode(__CHILDE_NODE__::RIGHT, this->rotateLeft(lnode));
							lnode = node->parent->getChildNode(__CHILDE_NODE__::LEFT);
						}
						lnode->color = lnode->parent->color;
						lnode->parent->color = __MRBTREE_NODE_COLOR_RED__;
						lnode->getChildNode(__CHILDE_NODE__::LEFT)->color = __MRBTREE_NODE_COLOR_BLACK__;

						// Ȼ�����������ڵ�ĸ��ڵ㣬���任������ֵܻ�������ʹ���ֵܽڵ�ĺ�ɫ�ӽڵ�ת����node��Ϊ���ڵ�����Ľڵ�
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
						// �ĳ�root ����while
						node = this->root;
					}
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

	// δ֪bug ���÷���д��MRBTreeʱ���򱨴� δ�������Լ�� �Ĵ���
	template<__Tree_Node_Inline_Ele_Type__ T>
	static MTree<T, MRBTree<T>>* MRBTree2MTree()
	{
		return new MTree< T, MRBTree<T>>;
	}
};

#endif // !__MUZI_MRBTREE_H__
