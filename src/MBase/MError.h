#ifndef  __MUZI_MERROR_H__
#define  __MUZI_MERROR_H__

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

	NOOBJECT,
	UNUSEFUL
};
#endif // ! __MUZI_MERROR_H__

