#ifndef __MYOBJCORE_H__
#define __MYOBJCORE_H__
#include "..\..\mysql\include\mysql.h"

const UINT MAX_ENUM_LENGTH = 2048;

class CMySqlDataConvertor
{
public:
	CMySqlDataConvertor() {};
	~CMySqlDataConvertor() {};

	BOOL ConvertType(enum_field_types eType,
		BOOL bUnsigned,
		UINT uSize,
		DBTYPE *pwType,
		UINT *puColumnSize,
		BOOL *pbFixedLength);

	BOOL ConvertData(DBTYPE wType,
		void *pData,
		UINT cbData,
		void *pConvertedData,
		UINT cbConvertedData,
		BOOL *pbTruncated);
};

/*
	Comment
	-------------
	ConvertData(...)
	[in]  wType - ���
	[in]  pData - ��������� ������������� ������
	[in]  cbData - ����� ���������� ������������� 
	[out] pConvertedData - ���������������� ������
	[in]  cbConvertedData - ����� ������ ��� ���������������� ������ 
		  ����� DBTYPE_STR|BYTES, ��� ������ ����� �� ������������;
		  ���� ������ ����� �� ���������� � ����� �������,
		  �� � ����� ���������� ������ �� �����,  � *bTruncated = 1
	[out] bTruncated
*/
#endif //__MYOBJCORE_H__

