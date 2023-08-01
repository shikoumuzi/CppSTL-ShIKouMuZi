#ifndef __MUZI_MRECVMSGNODE_H__
#define __MUZI_MRECVMSGNODE_H__
#include"MMsgNode.h"
namespace MUZI::net::async
{
	class MRecvMsgNode;
	using RecvMsgPackage = std::shared_ptr<MRecvMsgNode>;

	class MRecvMsgNode :public MMsgNode<__MUZI_MMSGNODE_PACKAGE_TYPE_RECV__>
	{
	public:
		static inline RecvMsgPackage getRecvMsgPackage(void* data = nullptr, uint64_t size = __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__)
		{
			return std::make_shared<MRecvMsgNode>(data, size);
		}

	public:
		MRecvMsgNode(void* data = nullptr, uint64_t size = __MUZI_MMSGNODE_MSGNODE_DEFAULT_ARG_SIZE__)
			:MMsgNode(data, size)
		{}
		MRecvMsgNode(MRecvMsgNode& node):MMsgNode(node), head_analyzed(node.head_analyzed)
		{}
	public:
		MMsgNodeDataBaseMsg& analyzeHeader()
		{
			if (!this->head_analyzed)
			{
				this->m_data->total_size = boost::asio::detail::socket_ops::host_to_network_long(static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->total_size);
				this->m_data->cur_size = 0;
				this->m_data->id = boost::asio::detail::socket_ops::host_to_network_long(static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id);
				
				static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->total_size = this->m_data->total_size;
				static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data)->msg_id = this->m_data->id;
				this->head_analyzed = true;
			}
			return *static_cast<MMsgNodeDataBaseMsg*>(this->m_data->data);
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
