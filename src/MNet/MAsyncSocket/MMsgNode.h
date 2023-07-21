#pragma once
#ifndef __MUZI_MASYNCSOCKET_MSGNODE_H__
#define __MUZI_MASYNCSOCKET_MSGNODE_H__
#include<stdint.h>
#include<memory>
namespace MUZI::net::async
{
	class MMsgNode
	{
	public:
		friend class MAsyncSocket;
	public:
		class MMsgNodeData
		{
		public:
			/// @brief 
			/// @param data nullptr if msgnode is read buffer
			/// @param size data size in bytes
			/// @param isBuffer true if need data hosting
			MMsgNodeData(void* data, uint64_t size, bool isBuffer)
				: total_size(size), cur_size(0), isBuffer(isBuffer)
			{
				this->data = static_cast<void*>(new char[size]);
				if (!isBuffer)
				{
					memcpy(this->data, data, size);
				}
			}
			~MMsgNodeData()
			{
				if (this->data != nullptr)
				{
					delete[] this->data;
				} 
				data = nullptr;
			}
		public:
			void* data;
			uint64_t cur_size;
			uint64_t total_size;
			bool isBuffer;
		};

	public:
		/// @brief 
		/// @param data nullptr if msgnode is read buffer
		/// @param size data size in bytes
		/// @param isBuffer true if need data hosting
		MMsgNode(void* data, uint64_t size, bool isBuffer = false)
			:m_data(new MMsgNodeData(data, size, isBuffer)) {}
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
