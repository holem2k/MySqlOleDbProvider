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
	[in]  wType - тип
	[in]  pData - строковое представление данных
	[in]  cbData - длина строкового представления 
	[out] pConvertedData - конвертированные данные
	[in]  cbConvertedData - длина буфера для конвертированных данных 
		  типов DBTYPE_STR|BYTES, для других типов не используется;
		  если данные имеют не помещаются в буфер целиком,
		  то в буфер помещается только их часть,  а *bTruncated = 1
	[out] bTruncated
*/
#endif //__MYOBJCORE_H__

