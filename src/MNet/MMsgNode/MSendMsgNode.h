#ifndef __MUZI_MSENDMSGNODE_H__
#define __MUZI_MSENDMSGNODE_H__
#include"MMsgNode.h"
#include<memory>
#include<rapidjson/rapidjson.h>
#include<rapidjson/prettywriter.h>
#include<rapidjson/stringbuffer.h>

namespace MUZI::net
{
	template<bool JSON_HEADER_SWITCH = __MUZI_MMSGNODE_PACKAGE_JSONFORMAT_OFF__>
	class MSendMsgNode;
	using SendRawMsgPackage = std::shared_ptr<MSendMsgNode<__MUZI_MMSGNODE_PACKAGE_JSONFORMAT_OFF__>>;
	using SendJsonMsgPackage = std::shared_ptr<MSendMsgNode<__MUZI_MMSGNODE_PACKAGE_JSONFORMAT_ON__>>;

	template<bool JSON_HEADER_SWITCH>
	class MSendMsgNode : public MMsgNode
	{
	public:
		static inline SendMsgPackage getSendMsgPackage(void* data, uint32_t size)
		{
			return std::make_shared<MSendMsgNode>(data, size);
		}
	public:
		MSendMsgNode(void* data, uint32_t size):MMsgNode(data, size)
		{
			this->m_data->total_size = size + 1 + sizeof(MMsgNodeDataBaseMsg);
			this->m_data->capacity = this->m_data->total_size;
			this->m_data->data = static_cast<void*>(new char[this->m_data->capacity] {'\0'});
			this->m_data->msg_size = size;
			//static_cast<char*>(this->m_data->data) + sizeof(MMsgNodeDataBaseMsg) + 1， json 与 数据段 用\0分割
			memcpy(static_cast<char*>(this->m_data->data) + sizeof(MMsgNodeDataBaseMsg) + 1, data, this->m_data->total_size - sizeof(MMsgNodeDataBaseMsg) - 1);

			if constexpr (JSON_HEADER_SWITCH)
			{
				rapidjson::StringBuffer buf;
				rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

				writer.StartObject();
				writer.Key("msg_id");
				writer.Uint(this->m_data->id);

				writer.Key("msg_size");
				writer.Uint(this->m_data->msg_size);

				writer.Key("total_size");
				writer.Uint(this->m_data->total_size);

				writer.EndObject();

				this->m_data->header_size = buf.GetSize();
				memcpy(this->m_data->data, buf.GetString(), this->m_data->header_size);
			
			}
			else
			{
				//取消息头部内容直接转换指针进行设置
				//写包需要本地序转网络序
				MMsgNodeDataBaseMsg* id_ptr = static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data);
				id_ptr->msg_id = 0;
				id_ptr->msg_size = boost::asio::detail::socket_ops::host_to_network_long(this->m_data->msg_size);
				id_ptr->total_size = boost::asio::detail::socket_ops::host_to_network_long(this->m_data->total_size);
			}

		}
		MSendMsgNode(const MSendMsgNode& node) :MMsgNode(node)
		{

		}

	public:
		

	
	};

	
}

#endif // !__MUZI_MSENDMSGNODE_H__
