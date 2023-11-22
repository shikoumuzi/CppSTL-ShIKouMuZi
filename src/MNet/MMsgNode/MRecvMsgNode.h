#ifndef __MUZI_MRECVMSGNODE_H__
#define __MUZI_MRECVMSGNODE_H__
#include"MMsgNode.h"
#include<rapidjson/prettywriter.h>
#include<rapidjson/stringbuffer.h>
#include<rapidjson/rapidjson.h>
#include<rapidjson/document.h>


namespace MUZI::net
{
	class MRecvMsgNode;

	using RecvMsgPackage = std::shared_ptr<MRecvMsgNode>;

	class MRecvMsgNode :public MMsgNode
	{
	public:
		static inline RecvMsgPackage getRecvMsgPackage(void* data = nullptr, uint64_t size = __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__)
		{
			return std::make_shared<MRecvMsgNode>(data, size);
		}

	public:
		MRecvMsgNode(void* data = nullptr, uint64_t size = __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__)
			:MMsgNode(data, size),
			head_analyzed(false)
		{
			// 读缓冲默认缓冲大小设置
			this->m_data->data = static_cast<void*>(new char[__MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__] {'\0'});
			this->m_data->capacity = __MUZI_MMSGNODE_PACKAGE_MAX_SIZE_IN_BYTES__;
		}
		MRecvMsgNode(MRecvMsgNode& node):MMsgNode(node), head_analyzed(node.head_analyzed)
		{}
	public:
		MMsgNodeDataBaseMsg analyzeHeader()
		{
			if (!this->head_analyzed)
			{
				// 头部解析
				// 裸数据
				
				// 取消息头部内容直接转换指针进行设置
				// 读包需要网络序转本地序
				this->m_data->total_size = boost::asio::detail::socket_ops::network_to_host_long(static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->total_size);
				//this->m_data->cur_size = 0;
				this->m_data->id = boost::asio::detail::socket_ops::network_to_host_long(static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id);
				this->m_data->msg_size = this->getTotalSize() - sizeof(MMsgNodeDataBaseMsg) - 1;

				this->head_analyzed = true;
			}

			return {.msg_id = this->m_data->id,
					.msg_size = this->m_data->msg_size,
					.total_size = this->m_data->total_size};
		}

	public:
		inline bool getHeadAnalyzed()
		{
			return this->head_analyzed;
		}
	public:
		void clear()
		{
			this->MMsgNode::clear();
			this->head_analyzed = false;
		}
	protected:
		bool head_analyzed;
	};

}

#endif // !__MUZI_MRECVMSGNODE_H__
