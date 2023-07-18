#pragma once
#ifndef __MUZI_MASYNCSOCKET_MSGNODE_H__
#define __MUZI_MASYNCSOCKET_MSGNODE_H__
#include<stdint.h>
#include<memory>
namespace MUZI::NET::ASYNC
{
	class MMsgNode
	{
	public:
		friend class MAsyncSocket;
	public:
		class MMsgNodeData
		{
		public:
			MMsgNodeData(void* data, uint64_t size, bool isNew)
				: data(static_cast<void*>(data)), total_size(size), cur_size(0), isNew(isNew) {}
			~MMsgNodeData()
			{
				if (isNew)
				{
					delete this->data;
				} 
				data = nullptr;
			}
		public:
			void* data;
			uint64_t cur_size;
			uint64_t total_size;
			bool isNew;
		};

	public:
		MMsgNode(void* data, uint64_t size, bool isNew = false)
			:m_data(new MMsgNodeData(data, size, isNew)) {}
		MMsgNode(const MMsgNode& msg)
		{
			this->m_data = msg.m_data;
		}
		MMsgNode(MMsgNode&& msg):m_data(std::move(msg.m_data))
		{}
		~MMsgNode()
		{
			this->m_data.reset();
		}
	public:
		inline void* getData() 
		{ 
			return this->m_data->data; 
		};
		inline uint64_t& getTotalSize()
		{
			return this->m_data->total_size;
		}
		inline uint64_t& getCurSize()
		{
			return this->m_data->cur_size;
		}
	public:
		inline bool isEmpty() { return this->m_data == nullptr; }
	private:
		std::shared_ptr<MMsgNodeData> m_data;
	};

	using MsgPackage = std::shared_ptr<MMsgNode>;

}
#endif // !__MUZI_MASYNCSOCKET_MSGNODE_H__
