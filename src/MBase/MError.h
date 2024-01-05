#ifndef  __MUZI_MERROR_H__
#define  __MUZI_MERROR_H__
#include<string>
#include<stdint.h>
enum MERROR
{
	MERROR_MAX = 1,

	// 完成并且无错
	COMPLETE = 0,
	FAILED = -1,

	// 文件系统
	NOADIR = -101,
	NOAFILE = -102,

	//文件系统错误
	DIRNOEXITS = -103,

	//sql错误
	SQLITE_OPEN_ERR = -201,
	SQLITE_CREATE_ERR = -202,
	SQLITE_CREATETABLE_ERR = -203,
	SQLITE_PREPARE_ERR = -204,
	SQL_NO_FOUND = -205,
	SQL_ID_ERR = -206,

	//输入参数错误
	ARG_IS_UNEXPECTED = -301,
	ARG_IS_NULL = -302,

	// 数据超出范围
	OUT_OF_RANGE = -303,
	OUT_OF_BUFF = -304,

	// 类型错误
	TYPE_ERR = -305,

	// 对象存在判断
	NOOBJECT = -306,
	UNUSEFUL = -307,
	OBJECT_IS_NULL = -308,

	// 网络
	IPADDRESS_FORMAT_CANNOT_ANALIZE = -401,
	ENDPOINT_IS_NO_CREATED = -402,
	RECV_ERROR = -403,
	SEND_ERROR = -404,
	READ_PENDING_NOW = -405,
	WRITE_PENDING_NOW = -406,

	OBJECT_IS_NO_SERVER = -407,
	OBJECT_IS_NO_CLIENT = -408,
	BIND_ERROR = -409,
	CONNECT_ERROR = -410,
	ACCEPT_ERROR = -411,
	LISTEN_ERROR = -412,
	SOCKET_SYSTEM_ERROR = -413,

	// 音视频
	MAV_STREAM_ID_NO_EXIT = -501,
	MAV_OBJECT_IS_NOT_USEFUL = -502,
	MAV_DECODER_NOT_FOUND = -503,
	MAV_DECODER_OPEN_FAILED = -504,
	MAV_DECODER_SEND_EOF = -505,
	MAV_DECODER_SEND_PACKAGE_FAILED = -506,
	MAV_DECODER_RECV_EOF = -507,
	MAV_DECODER_RECV_PACKAGE_FAILED = -508,
	MAV_READER_OPEN_INPUT_FAILED = -509,
	MAV_READER_FIND_STREAM_FAILED = -509,
	MAV_DECODER_PACKAGE_NOT_MATCH = -510,

	MERROR_MIN = INT_MIN,
};

#endif // ! __MUZI_MERROR_H__
