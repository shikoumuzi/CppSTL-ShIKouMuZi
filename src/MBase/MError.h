#ifndef  __MUZI_MERROR_H__
#define  __MUZI_MERROR_H__
#include<string>
enum MERROR
{
	// ��ɲ����޴�
	COMPLETE = 0,

	NOADIR,
	NOAFILE,
	
	//�ļ�ϵͳ����
	DIRNOEXITS,

	//sql����
	SQLITE_OPEN_ERR,
	SQLITE_CREATE_ERR,
	SQLITE_CREATETABLE_ERR,
	SQLITE_PREPARE_ERR,
	SQL_NO_FOUND,
	SQL_ID_ERR,

	//�����������
	ARG_IS_UNEXPECTED,

	NOOBJECT,
	UNUSEFUL
};

#endif // ! __MUZI_MERROR_H__

