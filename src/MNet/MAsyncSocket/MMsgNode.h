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
		struct MMsgNodeDataBaseMsg
		{
			uint64_t msg_id;
			uint64_t msg_size;
			uint64_t total_size;
		};

		class MMsgNodeData
		{
		public:

		public:
			/// @brief 
			/// @param data nullptr if msgnode is read buffer
			/// @param size data size in bytes
			/// @param isBuffer true if need data hosting
			MMsgNodeData(void* data, uint64_t size, bool isBuffer)
				: total_size(size + 1 + sizeof(MMsgNodeDataBaseMsg)), msg_size(size), cur_size(0), isBuffer(isBuffer)
			{
				this->data = static_cast<void*>(new char[size + sizeof(MMsgNodeDataBaseMsg)] {'\0'});
				if (!isBuffer)
				{
					memcpy(static_cast<char*>(this->data) + sizeof(MMsgNodeDataBaseMsg), data, size);
				}
				MMsgNodeDataBaseMsg* id_ptr = static_cast<MMsgNodeDataBaseMsg*>(this->data);
				id_ptr->msg_id = 0;
				id_ptr->msg_size = size;
				id_ptr->total_size = this->total_size;
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
			uint64_t msg_size;
			bool isBuffer;
			uint64_t id;
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
		inline void* getMsg()
		{
			return static_cast<char*>(this->m_data->data) + sizeof(MMsgNodeDataBaseMsg);
		}
		inline uint64_t& getTotalSize()
		{
			return this->m_data->total_size;
		}
		inline uint64_t& getCurSize()
		{
			return this->m_data->cur_size;
		}
		inline uint64_t& getMsgSize()
		{
			return this->m_data->msg_size;
		}
		inline uint64_t getId()
		{
			return static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id;
		}
		inline void setId(uint64_t& id)
		{
			static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id = id;
		}
		inline void setId(uint64_t id)
		{
			static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id = id;
		}
	public:
		void clear()
		{
			memset(this->m_data->data, '\0', this->m_data->total_size);
			this->m_data->cur_size = 0;
		}
	public:
		inline bool isEmpty() { return this->m_data == nullptr; }
	private:
		std::shared_ptr<MMsgNodeData> m_data;
	};

	using MsgPackage = std::shared_ptr<MMsgNode>;

}
#endif // !__MUZI_MASYNCSOCKET_MSGNODE_H__