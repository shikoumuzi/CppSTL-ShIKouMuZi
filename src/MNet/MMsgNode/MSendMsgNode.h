#ifndef __MUZI_MSENDMSGNODE_H__
#define __MUZI_MSENDMSGNODE_H__
#include"MMsgNode.h"
#include<memory>
#include<rapidjson/rapidjson.h>
#include<rapidjson/prettywriter.h>
#include<rapidjson/document.h>
#include<rapidjson/stringbuffer.h>
#include<stdio.h>
#include<stdlib.h>

namespace MUZI::net
{
	class MSendMsgNode;
	using SendRawMsgPackage = std::shared_ptr<MSendMsgNode>;

	using SendMsgPackage = std::shared_ptr<MUZI::net::MMsgNode>;

	class MSendMsgNode : public MMsgNode
	{
	public:
		//static inline SendMsgPackage getSendMsgPackage(void* data, uint32_t size)
		//{
		//	return std::make_shared<MSendMsgNode>(data, size);
		//}

	public:
		MSendMsgNode(const void* data, uint32_t size, int msg_id = 0):MMsgNode(data, size)
		{

			this->m_data->msg_size = size;
			//取消息头部内容直接转换指针进行设置
			//写包需要本地序转网络序
			this->m_data->total_size = size + 1 + sizeof(MMsgNodeDataBaseMsg);
			this->m_data->capacity = this->m_data->total_size;
			this->m_data->data = static_cast<void*>(new char[this->m_data->capacity] {'\0'});

			MMsgNodeDataBaseMsg* id_ptr = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data);
			id_ptr->msg_id = __MUZI_MASYNCSOCKET_SPECIFICAL_PACKAGE_HEADER_ID__;
			id_ptr->msg_size = boost::asio::detail::socket_ops::host_to_network_long(this->m_data->msg_size);
			id_ptr->total_size = boost::asio::detail::socket_ops::host_to_network_long(this->m_data->total_size);
			this->m_data->header_size = sizeof(MMsgNodeDataBaseMsg);
			memcpy(static_cast<char*>(this->m_data->data) + this->m_data->header_size, data, this->m_data->msg_size);

			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

			writer.StartObject();
			writer.Key("id");
			writer.Int(msg_id);
			writer.Key("data");
			writer.String(static_cast<const char*>(data), size);
			writer.EndObject();

			memcpy(static_cast<char*>(this->getData()) + this->m_data->header_size, buffer.GetString(), buffer.GetSize());
		}
		MSendMsgNode(const MSendMsgNode& node) :MMsgNode(node)
		{}		

	
	};

	
}

#endif // !__MUZI_MSENDMSGNODE_H__
