#ifndef __MUZI_MSENDMSGNODE_H__
#define __MUZI_MSENDMSGNODE_H__
#include"MMsgNode.h"
#include<memory>
namespace MUZI::net::async
{
	class MSendMsgNode;
	using SendMsgPackage = std::shared_ptr<MSendMsgNode>;
	class MSendMsgNode : public MMsgNode<__MUZI_MMSGNODE_PACKAGE_TYPE_SEND__>
	{
	public:
		static inline SendMsgPackage getSendMsgPackage(void* data, uint32_t size)
		{
			return std::make_shared<MSendMsgNode>(data, size);
		}
	public:
		MSendMsgNode(void* data, uint32_t size):MMsgNode(data, size)
		{

		}
		MSendMsgNode(const MSendMsgNode& node) :MMsgNode(node)
		{

		}
	};

	
}

#endif // !__MUZI_MSENDMSGNODE_H__
