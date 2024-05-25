#ifndef __MUZI_MLIST_H__
#define __MUZI_MLIST_H__
#include"../../MAllocator/MBitmapAllocator.h"
namespace MUZI
{
	template<typename T>
	class MList
	{
	private:
		template<typename T>
		struct __MList_Node__
		{
			T ele;
			__MList_Node__* parent;
			__MList_Node__* next;
		};
	public:
		MList():alloc(new MBitmapAllocator<T>()), root(nullptr){}
		MList(const MList<T>& that)
		{
			this->__delete__();
			struct __MList_Node__<T>* tmp_p1 = that.root;
			while (tmp_p1 != nullptr)
			{
				
			}
		}
		~MList()
		{
			delete
		}
	private:
		void __delete__()
		{
			struct __MList_Node__<T>* tmp_p1 = this->root, *tmp_p2 = this->root;
			while (tmp_p1 != nullptr)
			{
				tmp_p2 = tmp_p1;
				this->alloc->deallocate(tmp_p1);
				tmp_p1 = tmp_p2->next;
			}
			
		}
	private:
		MBitmapAllocator<T>* alloc;
		struct __MList_Node__<T>* root;
	};
}
#endif // !__MUZI_MLIST_H__
