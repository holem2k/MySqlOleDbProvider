#include "stdafx.h"
#include <stdio.h>
#include "myobjcore.h"


///////////////////////////////////////////////////////////////////////////
// CMySqlDataConvertor

BOOL CMySqlDataConvertor::ConvertType(enum_field_types eType,
									  BOOL bUnsigned,
									  UINT uSize,
									  DBTYPE *pwType,
									  UINT *puColumnSize,
									  BOOL *pbFixedLength)
{
	if (pwType == NULL || puColumnSize == NULL || pbFixedLength == NULL)
		return FALSE;
	
	// assume fixed length type
	*pbFixedLength = TRUE;

	BOOL bResult = TRUE;
	switch (eType)
	{
		// TINYINT
	case FIELD_TYPE_TINY:  
		*pwType = bUnsigned ? DBTYPE_UI1 : DBTYPE_I1;
		*puColumnSize = sizeof(char);
		break;

		// SMALLINT
	case FIELD_TYPE_SHORT: 
		*pwType = bUnsigned ? DBTYPE_UI2 : DBTYPE_I2;
		*puColumnSize = sizeof(short);
		break;

		// MEDIUMINT
	case FIELD_TYPE_INT24: 
		*pwType = bUnsigned ? DBTYPE_UI4 : DBTYPE_I4;
		*puColumnSize = sizeof(int);
		break;

		// INT
	case FIELD_TYPE_LONG: 
		*pwType = bUnsigned ? DBTYPE_UI4 : DBTYPE_I4;
		*puColumnSize = sizeof(int);
		break;

		// BIGINT
	case FIELD_TYPE_LONGLONG:  
		*pwType = bUnsigned ? DBTYPE_UI8 : DBTYPE_I8;
		*puColumnSize = sizeof(__int64);
		break;

		// FLOAT
	case FIELD_TYPE_FLOAT: 
		*pwType = DBTYPE_R4;
		*puColumnSize = sizeof(float);
		break;

		// DOUBLE
	case FIELD_TYPE_DOUBLE: 
		*pwType = DBTYPE_R8;
		*puColumnSize = sizeof(double);
		break;

		// DECIMAL
	case FIELD_TYPE_DECIMAL:
		*pwType = DBTYPE_R8;
		*puColumnSize = sizeof(double);
		break;

		// DATE
	case FIELD_TYPE_DATE:
		*pwType = DBTYPE_DBDATE;
		*puColumnSize = sizeof(DBDATE);
		break;

		// TIME
	case FIELD_TYPE_TIME:
		*pwType = DBTYPE_DBTIME;
		*puColumnSize = sizeof(DBTIME);
		break;

		// DATETIME
	case FIELD_TYPE_DATETIME:
		*pwType = DBTYPE_DBTIMESTAMP;
		*puColumnSize = sizeof(DBTIMESTAMP);
		break;

		// YEAR
	case FIELD_TYPE_YEAR:
		*pwType = DBTYPE_UI2;
		*puColumnSize = sizeof(unsigned short);
		break;

		// TIMESTAMP(6|8|10|12|14)
	case FIELD_TYPE_TIMESTAMP:
		*pwType = DBTYPE_DBTIMESTAMP;
		*puColumnSize = sizeof(DBTIMESTAMP);
		break;

		// CHAR(m) & VARCHAR(m)
	case FIELD_TYPE_VAR_STRING:
	case FIELD_TYPE_STRING:
		*pwType = DBTYPE_STR;
		*puColumnSize = uSize;
		*pbFixedLength = FALSE;
		break;

		// BLOB & TEXT
	case FIELD_TYPE_BLOB:
		*pwType = DBTYPE_BYTES;
		*puColumnSize = uSize;
		*pbFixedLength = FALSE;
		break;

		// ENUM
	case FIELD_TYPE_ENUM:
		*pwType = DBTYPE_STR;
		*puColumnSize = MAX_ENUM_LENGTH;
		break;
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlDataConvertor::ConvertData(DBTYPE wType,
		void *pData,
		UINT cbData,
		void *pConvertedData,
		UINT cbConvertedData,
		BOOL *pbTruncated)

{
	BOOL bResult = TRUE;
	*pbTruncated = FALSE;
	switch (wType)
	{
		// NUMERIC
	case DBTYPE_UI1:
		{
			unsigned int i = 0;
			bResult = sscanf((char *)pData, "%u", &i);
			*(unsigned char *)pConvertedData = i;
		}
		break;
		
	case DBTYPE_I1:
		{
			int i = 0;
			bResult = sscanf((char *)pData, "%d", &i);
			*(char *)pConvertedData = i;
		}
		break;
		
	case DBTYPE_UI2:
		bResult = sscanf((char *)pData, "%hu", (unsigned short *)pConvertedData);
		break;
		
	case DBTYPE_I2:
		bResult = sscanf((char *)pData, "%hd", (unsigned short *)pConvertedData);
		break;

	case DBTYPE_UI4:
		bResult = sscanf((char *)pData, "%u", (unsigned int *)pConvertedData);
		break;
		
	case DBTYPE_I4:
		bResult = sscanf((char *)pData, "%d", (int *)pConvertedData);
		break;

	case DBTYPE_UI8:
		bResult = sscanf((char *)pData, "%I64u", (unsigned __int64 *)pConvertedData);
		break;
		
	case DBTYPE_I8:
		bResult = sscanf((char *)pData, "%I64d", (__int64 *)pConvertedData);
		break;

	case DBTYPE_R4:
		bResult = sscanf((char *)pData, "%f", (float *)pConvertedData);
		break;

	case DBTYPE_R8:
		bResult = sscanf((char *)pData, "%lf", (double *)pConvertedData);
		break;
/*
	case DBTYPE_DECIMAL:
		{
			bResult = FALSE;

			unsigned char bDigital[128];
			USES_CONVERSION;
			NUMPARSE nump;
			nump.dwInFlags = NUMPRS_STD;
			nump.cDig = sizeof(bDigital);
			HRESULT hr = VarParseNumFromStr(A2OLE((char *)pData),
				0, NUMPRS_STD, &nump, (unsigned char *)bDigital);

			if (hr == S_OK)
			{
				VARIANT varDecimal;
				VariantInit(&varDecimal);
				if (VarNumFromParseNum(&nump, bDigital,
					VTBIT_DECIMAL, &varDecimal) == S_OK)
				{
					MoveMemory(pConvertedData, &varDecimal.decVal, sizeof(varDecimal.decVal));
					bResult = TRUE;
				}
			}
		}
		break;
*/
		// DATE and TIME
	case DBTYPE_DBDATE:
		{
			DBDATE date;
			bResult = sscanf((char *)pData, "%hd-%hu-%hu",
				&date.year, &date.month, &date.day) == 3;
			if (bResult)
				MoveMemory(pConvertedData, &date, sizeof(date));
		}
		break;

	case DBTYPE_DBTIME:
		{
			DBTIME time;
			bResult = sscanf((char *)pData, "%hd-%hu-%hu",
				&time.hour, &time.minute, &time.second) == 3;
			if (bResult)
				MoveMemory(pConvertedData, &time, sizeof(time));
		}
		break;

	case DBTYPE_DBTIMESTAMP:
		{
			bResult = FALSE;
			DBTIMESTAMP ts;
			ZeroMemory(&ts, sizeof(ts));
			if (sscanf((char *)pData, "%hd-%hd-%hd %hd:%hd:%hd", &ts.year, &ts.month, &ts.day,
				&ts.hour, &ts.minute, &ts.second) == 6)
			{
				bResult = TRUE;
			}
			else
			{
				// MySql TIMESTAMP ?
				switch(lstrlen((char *)pData))
				{
				case 14:
					bResult = sscanf((char *)pData, "%4hd%2hu%2hu%2hu%2hu%2hu", &ts.year, &ts.month, &ts.day,
						&ts.hour, &ts.minute, &ts.second) == 6;
					break;

				case 12:
					bResult = sscanf((char *)pData, "%2hd%2hu%2hu%2hu%2hu%2hu", &ts.year, &ts.month, &ts.day,
						&ts.hour, &ts.minute, &ts.second) == 6;
					if (bResult)
					{
						if (ts.year < 70) 
							ts.year += 2000;
						else 
							ts.year += 1900;
					}
					break;

				case 8:
					bResult = sscanf((char *)pData, "%4hd%2hu%2hu", &ts.year, &ts.month, &ts.day) == 3;
					break;

				case 6:
					bResult = sscanf((char *)pData, "%2hd%2hu%2hu", &ts.year, &ts.month, &ts.day) == 3;
					if (bResult)
					{
						if (ts.year < 70) 
							ts.year += 2000;
						else 
							ts.year += 1900;
					}
					break;
				}
				if (bResult)
					MoveMemory(pConvertedData, &ts, sizeof(ts));
			}
		} // case DBTYPE_DBTIMESTAMP:
		break;
		
	case DBTYPE_BYTES:
		{
			UINT cbLen;
			if (cbData <= cbConvertedData)
				cbLen = cbData;
			else
			{
				cbLen = cbConvertedData;
				*pbTruncated = TRUE;
			}
			// write BLOB length before BLOB data
			*(ULONG *)pConvertedData = cbLen;
			MoveMemory((ULONG *)pConvertedData + 1, pData, cbLen);
		}
		bResult = TRUE;
		break;
		
	case DBTYPE_STR:
		{
			UINT cbLen;
			if (cbData + 1 <= cbConvertedData)
				cbLen = cbData;
			else
			{
				cbLen = cbConvertedData;
				*pbTruncated = TRUE;
			}
			MoveMemory(pConvertedData, pData, cbLen);
			((char *)pConvertedData)[cbLen] = 0;
			bResult = TRUE;
		}
		break;
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////
