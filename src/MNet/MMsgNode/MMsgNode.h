#pragma once
#ifndef __MUZI_MASYNCSOCKET_MSGNODE_H__
#define __MUZI_MASYNCSOCKET_MSGNODE_H__
#include<stdint.h>
#include<memory>
#include<boost/asio.hpp>
#include<rapidjson/rapidjson.h>
#include<rapidjson/prettywriter.h>
#include<rapidjson/stringbuffer.h>

#define __MUZI_MMSGNODE_PACKAGE_JSONFORMAT_ON__ true
#define __MUZI_MMSGNODE_PACKAGE_JSONFORMAT_OFF__ false
#define __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__ sizeof(MMsgNodeDataBaseMsg)
#define __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_LEAST__ 45
#define __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_MOST__ 72
#define __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__ 1400
#define __MUZI_MMSGNODE_MSGNODE_DEFAULT_MAX_MAG_ID__ 16384
#define __MUZI_MMSGNODE_RAW_HEADER_PACKAGE_MAX_SIZE_IN_BYTES__ __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__ + __MUZI_MMSGNODE_MSGNODE_RAW_HEADER_SIZE_IN_BYTES__
#define __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_LEAST__ __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__ + __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_LEAST__
#define __MUZI_MMSGNODE_JSON_HEADER_PACKAGE_MAX_SIZE_IN_BYTES_AT_MOST__ __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__ + __MUZI_MMSGNODE_MSGNODE_JSON_HEADER_SIZE_IN_BYTES_AT_MOST__


namespace MUZI::net
{
	struct MMsgNodeDataBaseMsg
	{
		uint32_t msg_id;
		uint32_t msg_size;
		uint32_t total_size;
	};

	class __declspec(novtable) MMsgNode
	{
	public:
		friend class MAsyncSocket;
	public:

		class MMsgNodeData
		{
		public:

		public:
			/// @brief 
			/// @param data nullptr if msgnode is read buffer
			/// @param size data size in bytes
			/// @param isBuffer true if need data hosting
			MMsgNodeData(const void* data, uint64_t size)
				: total_size(0), 
				msg_size(0), 
				cur_size(0), 
				id(0),
				capacity(0),
				header_size(0),
				data(nullptr)
			{

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
			void* data;// 数据信息
			uint32_t cur_size;// 现在读取/写入的长度
			uint32_t total_size;// 总的包长度（信息+头部）
			uint32_t msg_size;// 信息长度
			uint32_t capacity;// 容量
			uint32_t id;// id
			uint32_t header_size;
		};

	public:
		/// @brief 
		/// @param data nullptr if msgnode is read buffer
		/// @param size data size in bytes
		/// @param isBuffer true if need data hosting
		MMsgNode(const void* data, uint64_t size = __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__)
			:m_data(new MMsgNodeData(data, size)) {}
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
			return static_cast<char*>(this->m_data->data) + this->m_data->header_size;
		}
		inline uint32_t& getTotalSize()
		{
			return this->m_data->total_size;
		}
		inline uint32_t& getCurSize()
		{
			return this->m_data->cur_size;
		}
		inline uint32_t& getMsgSize()
		{
			return this->m_data->msg_size;
		}
		inline uint32_t getId()
		{
			return static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id;
		}
		inline void setId(uint32_t id)
		{
			static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id = id;
			this->m_data->id = id;
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
		//MMsgNodeDataBaseMsg& analyzeHeader()
		//{
		//	//struct MMsgNodeDataBaseMsg
		//	//{
		//	//	uint64_t msg_id;
		//	//	uint64_t msg_size;
		//	//	uint64_t total_size;
		//	//};
		//	//return {.msg_id = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id,
		//	//		.msg_size = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_size,
		//	//		.total_size = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->total_size};
		//	if (!this->m_data->head_analyzed)
		//	{
		//		this->m_data->total_size = boost::asio::detail::socket_ops::network_to_host_long(static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->total_size);
		//		this->m_data->cur_size = 0;
		//		this->m_data->id = boost::asio::detail::socket_ops::network_to_host_long(static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id);

		//		static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->total_size = this->m_data->total_size;
		//		static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id = this->m_data->id;
		//		this->m_data->head_analyzed = true;
		//	}
		//	return *static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data);
		//}
	public:
		inline bool isEmpty() { return this->m_data == nullptr; }

	private:
		uint32_t getJsonFormatHeaderSize()
		{
			if (this->m_data->header_size != 0)
			{
				return this->m_data->header_size;
			}

			rapidjson::StringBuffer buf;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

			writer.StartObject();
			writer.Key("msg_id");
			writer.Uint(0);

			writer.Key("msg_size");
			writer.Uint(0);

			writer.Key("total_size");
			writer.Uint(0);

			writer.EndObject();

			this->m_data->header_size = buf.GetSize();
			return this->m_data->header_size;
		}
	protected:
		std::shared_ptr<MMsgNodeData> m_data;
	};

	using MsgPackage = std::shared_ptr<MMsgNode>;


}
#endif // !__MUZI_MASYNCSOCKET_MSGNODE_H__
