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

	NOOBJECT,
	UNUSEFUL
};

#endif // ! __MUZI_MERROR_H__

