#ifndef  __MUZI_MERROR_H__
#define  __MUZI_MERROR_H__
#include<string>
enum MERROR
{
	// 完成并且无错
	COMPLETE = 0,

	NOADIR,
	NOAFILE,

	//文件系统错误
	DIRNOEXITS,

	//sql错误
	SQLITE_OPEN_ERR,
	SQLITE_CREATE_ERR,
	SQLITE_CREATETABLE_ERR,
	SQLITE_PREPARE_ERR,
	SQL_NO_FOUND,
	SQL_ID_ERR,

	//输入参数错误
	ARG_IS_UNEXPECTED,
	ARG_IS_NULL,

	// 数据超出范围
	OUT_OF_RANGE,
	OUT_OF_BUFF,

	// 类型错误
	TYPE_ERR,

	// 对象存在判断
	NOOBJECT,
	UNUSEFUL,
	OBJECT_IS_NULL,

	// 网络
	IPADDRESS_FORMAT_CANNOT_ANALIZE,
	ENDPOINT_IS_NO_CREATED,
	RECV_ERROR,
	SEND_ERROR,
	READ_PENDING_NOW,
	WRITE_PENDING_NOW,

	OBJECT_IS_NO_SERVER,
	OBJECT_IS_NO_CLIENT,
	BIND_ERROR,
	CONNECT_ERROR,
	ACCEPT_ERROR,
	LISTEN_ERROR,
	SOCKET_SYSTEM_ERROR,

};

#endif // ! __MUZI_MERROR_H__

