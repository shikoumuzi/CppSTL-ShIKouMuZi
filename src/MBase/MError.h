#ifndef  __MUZI_MERROR_H__
#define  __MUZI_MERROR_H__
#include<string>
#include<stdint.h>

#define MUZI_ERROR_TO_STRING(x) #x
enum MERROR
{
	MERROR_MAX = 1,

	// ��ɲ����޴�
	COMPLETE = 0,
	FAILED = -1,

	// �ļ�ϵͳ
	NOADIR = -101,
	NOAFILE = -102,

	//�ļ�ϵͳ����
	DIRNOEXITS = -103,

	//sql����
	SQLITE_OPEN_ERR = -201,
	SQLITE_CREATE_ERR = -202,
	SQLITE_CREATETABLE_ERR = -203,
	SQLITE_PREPARE_ERR = -204,
	SQL_NO_FOUND = -205,
	SQL_ID_ERR = -206,

	//�����������
	ARG_IS_UNEXPECTED = -301,
	ARG_IS_NULL = -302,

	// ���ݳ�����Χ
	OUT_OF_RANGE = -303,
	OUT_OF_BUFF = -304,

	// ���ʹ���
	TYPE_ERR = -305,

	// ��������ж�
	NOOBJECT = -306,
	UNUSEFUL = -307,
	OBJECT_IS_NULL = -308,

	// ����
	MNET_IPADDRESS_FORMAT_CANNOT_ANALIZE = -401,
	MNET_ENDPOINT_IS_NO_CREATED = -402,
	MNET_RECV_ERROR = -403,
	MNET_SEND_ERROR = -404,
	MNET_READ_PENDING_NOW = -405,
	MNET_WRITE_PENDING_NOW = -406,

	MNET_OBJECT_IS_NO_SERVER = -407,
	MNET_OBJECT_IS_NO_CLIENT = -408,
	MNET_BIND_ERROR = -409,
	MNET_CONNECT_ERROR = -410,
	MNET_ACCEPT_ERROR = -411,
	MNET_LISTEN_ERROR = -412,
	MNET_SOCKET_SYSTEM_ERROR = -413,

	// ����Ƶ
	MAV_STREAM_ID_NO_EXIT = -501, // stream id ������
	MAV_OBJECT_IS_NOT_USEFUL = -502, // �ö��󲻿���
	MAV_DECODER_NOT_FOUND = -503, // ������δ�ҵ�
	MAV_DECODER_OPEN_FAILED = -504, // ��������ʧ��
	MAV_DECODER_SEND_PACKAGE_EOF = -505, // ������send package ����ĩβ
	MAV_DECODER_SEND_PACKAGE_FAILED = -506, // ������ send package ʧ��
	MAV_DECODER_RECV_FRAME_EOF = -507,  // ������ recv frame ����ĩβ
	MAV_DECODER_RECV_PACKAGE_FAILED = -508, // ������ recv frame ʧ��
	MAV_READER_OPEN_INPUT_FAILED = -509, // reader ��������ʧ��
	MAV_READER_FIND_STREAM_FAILED = -509, // reader ������ʧ��
	MAV_DECODER_PACKAGE_NOT_MATCH = -510, // ������ ���󶨵�streamͬ ��Ҫsend��package ��ƥ��
	MAV_READER_TARGET_TYPE_STREAM_IS_NOT_FOUND = -511, // readerѰ��Ŀ�����͵���ʱ����ʧ��
	MAV_READER_TARGET_TYPE_STREAM_HAVE_NOT_MATCHED_DECODER = -512, // readerѰ��Ŀ�����͵���ʱ����ʧ��

	MERROR_MIN = INT_MIN,
};

#endif // ! __MUZI_MERROR_H__
