#pragma once
#ifndef __MUZI_MASYNCSOCKET_MSGNODE_H__
#define __MUZI_MASYNCSOCKET_MSGNODE_H__
#include<stdint.h>
#include<memory>

#define __MUZI_MMSGNODE_MSGNODE_HEAD_SIZE_IN_BYTES__ sizeof(class MMsgNode::MMsgNodeDataBaseMsg)
#define __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__ 1400 + __MUZI_MMSGNODE_MSGNODE_HEAD_SIZE_IN_BYTES__
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
				: total_size(size + 1 + sizeof(MMsgNodeDataBaseMsg)), 
				msg_size(size), 
				cur_size(0), 
				isBuffer(isBuffer), 
				id(0)
			{
				if (isBuffer)// 读缓存
				{
					this->data = static_cast<void*>(new char[__MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__] {'\0'});
					this->total_size = (this->total_size > this->capacity) ? this->capacity : this->total_size;

					// 取消息头部内容直接转换指针进行设置
					MMsgNodeDataBaseMsg* id_ptr = static_cast<MMsgNodeDataBaseMsg*>(this->data);
					id_ptr->msg_id = 0;
					id_ptr->msg_size = this->total_size - sizeof(MMsgNodeDataBaseMsg) - 1;
					id_ptr->total_size = this->total_size;
					this->capacity = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__;
				}
				else
				{
					this->capacity = this->total_size;
					this->data = static_cast<void*>(new char[this->capacity] {'\0'});
					memcpy(static_cast<char*>(this->data) + sizeof(MMsgNodeDataBaseMsg), data, this->total_size - sizeof(MMsgNodeDataBaseMsg) - 1);

					// 取消息头部内容直接转换指针进行设置
					MMsgNodeDataBaseMsg* id_ptr = static_cast<MMsgNodeDataBaseMsg*>(this->data);
					id_ptr->msg_id = 0;
					id_ptr->msg_size = this->total_size - sizeof(MMsgNodeDataBaseMsg) - 1;
					id_ptr->total_size = this->total_size;
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
			uint64_t msg_size;
			uint64_t capacity;
			bool isBuffer;
			uint64_t id;

		};

	public:
		/// @brief 
		/// @param data nullptr if msgnode is read buffer
		/// @param size data size in bytes
		/// @param isBuffer true if need data hosting
		MMsgNode(void* data, uint64_t size = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__, bool isBuffer = false)
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
			return static_cast<char*>(this->m_data->data) + __MUZI_MMSGNODE_MSGNODE_HEAD_SIZE_IN_BYTES__;
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
			this->m_data->id = id;
		}
		inline void setId(uint64_t id)
		{
			static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id = id;
			this->m_data->id = id;
		}
		inline bool isBuffer()
		{
			return this->m_data->isBuffer;
		}

		//bool set(void* data, uint64_t size)
		//{
		//	if (size > this->m_data->capacity)
		//	{
		//		return false;
		//	}
		//	memcpy(this->m_data->data, data, size);
		//	this->m_data->cur_size = 0;
		//	this->m_data->id = 0;
		//	this->m_data->head_parse = false;
		//	this->m_data->total_size = size + 1 + sizeof(MMsgNodeDataBaseMsg);
		//	this->m_data->total_size = (this->m_data->total_size > this->m_data->capacity)? this->m_data->capacity: this->m_data->total_size;
		//}

	public:
		void clear()
		{
			memset(this->m_data->data, '\0', this->m_data->total_size);
			this->m_data->cur_size = 0;
			this->m_data->id = 0;
		}
		MMsgNodeDataBaseMsg& analyzeHeader()
		{
			//struct MMsgNodeDataBaseMsg
			//{
			//	uint64_t msg_id;
			//	uint64_t msg_size;
			//	uint64_t total_size;
			//};
			//return {.msg_id = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id,
			//		.msg_size = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_size,
			//		.total_size = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->total_size};
			
			return *static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data);
		}
	public:
		inline bool isEmpty() { return this->m_data == nullptr; }
	private:
		std::shared_ptr<MMsgNodeData> m_data;
	};

	using MsgPackage = std::shared_ptr<MMsgNode>;


}
#endif // !__MUZI_MASYNCSOCKET_MSGNODE_H__
