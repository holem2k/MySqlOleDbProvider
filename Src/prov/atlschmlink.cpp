#include "stdafx.h"
#include "atlschmlink.h"
#include "error.h"
#include "stdio.h"

/////////////////////////////////////////////////////////////////////////

void CopyProviderTypeData(CPROVIDER_TYPERowEx *pRow, const PROVIDERTYPE_DATA *pData)
{
	wcsncpy(pRow->m_szName, pData->m_szName, sizeof(pRow->m_szName)/sizeof(pRow->m_szName[0]));
	pRow->m_nType = pData->m_nType;
	pRow->m_ulSize = pData->m_ulSize;
	wcsncpy(pRow->m_szPrefix, pData->m_szPrefix, sizeof(pRow->m_szPrefix)/sizeof(pRow->m_szPrefix[0]));
	wcsncpy(pRow->m_szSuffix, pData->m_szSuffix, sizeof(pRow->m_szSuffix)/sizeof(pRow->m_szSuffix[0]));
	wcsncpy(pRow->m_szCreateParams, pData->m_szCreateParams, sizeof(pRow->m_szCreateParams)/sizeof(pRow->m_szCreateParams[0]));
	pRow->m_bIsNullable = pData->m_bIsNullable;
	pRow->m_bCaseSensitive = pData->m_bCaseSensitive;
	pRow->m_bSearchable = pData->m_bSearchable;
	pRow->m_bUnsignedAttribute = pData->m_bUnsignedAttribute;
	pRow->m_bFixedPrecScale = pData->m_bFixedPrecScale;
	pRow->m_bAutoUniqueValue = pData->m_bAutoUniqueValue;
	wcsncpy(pRow->m_szLocalTypeName, pData->m_szLocalTypeName, sizeof(pRow->m_szLocalTypeName)/sizeof(pRow->m_szName[0]));
	pRow->m_nMinScale = pData->m_nMinScale;
	pRow->m_nMaxScale = pData->m_nMaxScale;
	pRow->m_guidType = pData->m_guidType;
	wcsncpy(pRow->m_szTypeLib, pData->m_szTypeLib, sizeof(pRow->m_szTypeLib)/sizeof(pRow->m_szTypeLib[0]));	
	wcsncpy(pRow->m_szVersion, pData->m_szVersion, sizeof(pRow->m_szVersion)/sizeof(pRow->m_szVersion[0]));	
	pRow->m_bIsLong = pData->m_bIsLong;
	pRow->m_bBestMatch = pData->m_bBestMatch;
	pRow->m_bIsFixedLength = pData->m_bIsFixedLength;
}

/////////////////////////////////////////////////////////////////////////
// CMetaEnumerator

BOOL CMetaEnumerator::EnumDatabases(CMySqlConnection *pConnection,
									UINT *pcDatabases, 
									DATABASEINFO **ppInfo)
{
	USES_CONVERSION;
	if (pcDatabases == NULL || ppInfo == NULL)
	{
		if (pcDatabases)
			*pcDatabases = 0;
		if (ppInfo)
			*ppInfo = NULL;
		return FALSE;
	}

	// assume failure
	BOOL bResult = FALSE;
	*pcDatabases = NULL;
	*ppInfo = NULL;

	CMySqlCommand command;
	LONG cRowsAffected;
	CMySqlResult *pResult;
	bResult = command.Execute("SHOW DATABASES", pConnection, RESULT_STATIC, &cRowsAffected, &pResult);
	if (bResult)
	{
		unsigned __int64 uRowCount;
		if (pResult->RowCount(&uRowCount))
		{
			DATABASEINFO *pInfo = (DATABASEINFO *)CoTaskMemAlloc(sizeof(DATABASEINFO) * (ULONG)uRowCount);
			if (pInfo)
			{
				for (ULONG i = 0; i < (ULONG)uRowCount; i++)
				{
					if (!pResult->FetchRow())
						break;

					UINT cbData;
					BOOL bNull, bTruncated;
					char lpDatabase[MYSQL_METANAMELEN + 1];
					bResult = pResult->GetData(1,
						DBTYPE_STR,
						lpDatabase, 
						MYSQL_METANAMELEN + 1, 
						&cbData,
						&bNull,
						&bTruncated);
					if (!bResult)
						break;
					wcscpy(pInfo[i].szDatabase, A2OLE(lpDatabase));
				}
			}
			else
			{
				CreateError(ERRORSRC, OLESTR("Out of memory"), IID_IDBSchemaRowset);
				bResult = FALSE;
			}

			if (bResult)
			{
				*pcDatabases = (UINT)uRowCount;
				*ppInfo = pInfo;
			}
		}
		delete pResult;
	}
	else
		CreateError(ERRORSRC, A2OLE(pConnection->GetError()), IID_IDBSchemaRowset);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////

BOOL CMetaEnumerator::EnumTables(CMySqlConnection *pConnection,
								 LPOLESTR szDatabase,
								 UINT *pcTables,
								 TABLEINFO **ppInfo)
{
	USES_CONVERSION;
	if (pcTables == NULL || ppInfo == NULL)
	{
		if (pcTables)
			*pcTables = 0;
		if (ppInfo)
			*ppInfo = NULL;
		return FALSE;
	}

	// assume failure
	BOOL bResult = FALSE;
	*pcTables = NULL;
	*ppInfo = NULL;

	TCHAR lpCommand[2*MYSQL_METANAMELEN + 1];
	lstrcpy(lpCommand, "SHOW TABLES FROM ");
	lstrcat(lpCommand, OLE2T(szDatabase));
	CMySqlCommand command;
	LONG cRowsAffected;
	CMySqlResult *pResult;
	bResult = command.Execute(lpCommand, pConnection, RESULT_STATIC, &cRowsAffected, &pResult);
	if (bResult)
	{
		unsigned __int64 uRowCount;
		if (pResult->RowCount(&uRowCount))
		{
			TABLEINFO *pInfo = (TABLEINFO *)CoTaskMemAlloc(sizeof(TABLEINFO) * (ULONG)uRowCount);
			if (pInfo)
			{
				for (ULONG i = 0; i < (ULONG)uRowCount; i++)
				{
					if (!pResult->FetchRow())
						break;

					UINT cbData;
					BOOL bNull, bTruncated;
					char lpTable[MYSQL_METANAMELEN + 1];
					bResult = pResult->GetData(1,
						DBTYPE_STR,
						lpTable, 
						MYSQL_METANAMELEN + 1, 
						&cbData,
						&bNull,
						&bTruncated);
					if (!bResult)
						break;
					wcscpy(pInfo[i].szTable, A2OLE(lpTable));
				}
			}
			else
			{
				CreateError(ERRORSRC, OLESTR("Out of memory"), IID_IDBSchemaRowset);
				bResult = FALSE;
			}

			if (bResult)
			{
				*pcTables = (UINT)uRowCount;
				*ppInfo = pInfo;
			}
		}
		delete pResult;
	}
	else
		CreateError(ERRORSRC, A2OLE(pConnection->GetError()), IID_IDBSchemaRowset);
	return bResult;
}


/////////////////////////////////////////////////////////////////////////

BOOL CMetaEnumerator::EnumColumns(CMySqlConnection *pConnection, LPOLESTR szDatabase, LPOLESTR szTable,
		UINT *pcColumns,  SCHEMA_COLUMNINFO **ppInfo)

{
	USES_CONVERSION;
	if (pcColumns == NULL || ppInfo == NULL)
	{
		if (pcColumns)
			*pcColumns = 0;
		if (ppInfo)
			*ppInfo = NULL;
		return FALSE;
	}

	BOOL bResult = TRUE;
	// make command text
	TCHAR lpCommand[3*MYSQL_METANAMELEN + 1];
	lstrcpy(lpCommand, _TEXT("DESCRIBE "));
	lstrcat(lpCommand, OLE2T(szDatabase));
	lstrcat(lpCommand, _TEXT("."));
	lstrcat(lpCommand, OLE2T(szTable));
	// execute command
	CMySqlCommand command;
	LONG cRowsAffected;
	CMySqlResult *pResult;
	if (command.Execute(lpCommand, pConnection, RESULT_STATIC, &cRowsAffected, &pResult))
	{
		unsigned __int64 uRowCount;
		if (pResult->RowCount(&uRowCount))
		{
			SCHEMA_COLUMNINFO *pInfo = (SCHEMA_COLUMNINFO *)CoTaskMemAlloc(sizeof(SCHEMA_COLUMNINFO) * (ULONG)uRowCount);
			if (pInfo)
			{
				for (UINT i = 0; i < (ULONG)uRowCount; i++)
				{
					BOOL bRowResult = FALSE;
					char lpColumnName[MYSQL_METANAMELEN + 1];
					char lpColumnType[MYSQL_METANAMELEN + 1];
					char lpColumnNull[MYSQL_METANAMELEN + 1];
					char lpColumnKey[MYSQL_METANAMELEN + 1];
					char lpColumnDefault[MYSQL_METADEFAULTLEN + 1];
					char lpColumnExtra[MYSQL_METANAMELEN + 1];
					if (ReadRawRow(pResult, i,
						lpColumnName, lpColumnType,	lpColumnNull,
						lpColumnKey, lpColumnDefault, lpColumnExtra))
					{
						bRowResult = ConvertRawRow(lpColumnName, lpColumnType, lpColumnNull,
							lpColumnKey, lpColumnDefault, lpColumnExtra, pInfo, i);
					}
					if (!bRowResult)
					{
						bResult = FALSE;
						break;
					}
				}
			}
			else
			{
				CreateError(ERRORSRC, OLESTR("Out of memory"), IID_IDBSchemaRowset);
				bResult = FALSE;
			}

			if (bResult)
			{
				*pcColumns = (UINT)uRowCount;
				*ppInfo = pInfo;
			}
		}
		delete pResult;
	}
	else
		CreateError(ERRORSRC, A2OLE(pConnection->GetError()), IID_IDBSchemaRowset);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////

BOOL CMetaEnumerator::ReadRawRow(CMySqlResult *pResult, UINT nRow,
							  char *lpColumnName, char *lpColumnType,
							  char *lpColumnNull, char *lpColumnKey,
							  char *lpColumnDefault, char *lpColumnExtra)
{
	if (pResult->MoveAt(nRow))
	{
		if (pResult->FetchRow())
		{
			UINT cbData;
			BOOL bTruncated, bNull;
			// read column name (can't be NULL)
			if (!pResult->GetData(1, DBTYPE_STR, lpColumnName, MYSQL_METANAMELEN + 1,
				&cbData, &bNull, &bTruncated))
				return FALSE;
			if (bTruncated || bNull)
				return FALSE;

			// read column type (can't be NULL)
			if (!pResult->GetData(2, DBTYPE_STR, lpColumnType, MYSQL_METANAMELEN + 1,
				&cbData, &bNull, &bTruncated))
				return FALSE;
			if (bTruncated || bNull)
				return FALSE;

			// read column null (can't be NULL)
			if (!pResult->GetData(3, DBTYPE_STR, lpColumnNull, MYSQL_METANAMELEN + 1,
				&cbData, &bNull, &bTruncated))
				return FALSE;
			if (bTruncated || bNull)
				return FALSE;

			// read column key
			if (!pResult->GetData(4, DBTYPE_STR, lpColumnKey, MYSQL_METADEFAULTLEN + 1,
				&cbData, &bNull, &bTruncated))
				return FALSE;
			if (bTruncated)
				return FALSE;
			else if (bNull)
				lpColumnKey[0] = 0;

			// read column default value
			if (!pResult->GetData(5, DBTYPE_STR, lpColumnDefault, MYSQL_METADEFAULTLEN + 1,
				&cbData, &bNull, &bTruncated))
				return FALSE;
			if (bTruncated)
				return FALSE;
			else if (bNull)
				lpColumnDefault[0] = 0;

			// read column extra info
			if (!pResult->GetData(6, DBTYPE_STR, lpColumnExtra, MYSQL_METANAMELEN + 1,
				&cbData, &bNull, &bTruncated))
				return FALSE;
			if (bTruncated)
				return FALSE;
			else if (bNull)
				lpColumnExtra[0] = 0;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CMetaEnumerator::ConvertRawRow(char *lpColumnName, char *lpColumnType,
		char *lpColumnNull, char *lpColumnKey,
		char *lpColumnDefault, char *lpColumnExtra, SCHEMA_COLUMNINFO *pInfo, UINT nRow)
{
	USES_CONVERSION;
	SCHEMA_COLUMNINFO *pCurInfo = pInfo + nRow;
	pCurInfo->uOrdinal = nRow + 1;
	wcsncpy(pCurInfo->szColumn, A2OLE(lpColumnName), sizeof(pCurInfo->szColumn)/sizeof(OLECHAR));
	ParseDefaultStr(lpColumnDefault, &pCurInfo->bHasDefault, pCurInfo->szDefault);
	ParseNullStr(lpColumnNull, &pCurInfo->bIsNullable);
	ParseKeyStr(lpColumnKey, &pCurInfo->bPrimary, &pCurInfo->bForeign);
	if (!ParseTypeStr(lpColumnType, &pCurInfo->wType, &pCurInfo->uMaxLen, &pCurInfo->bFixLen))
		return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *puMaxLen == 0 if numeric or time type

BOOL CMetaEnumerator::ParseTypeStr(char *lpTypeStr, DBTYPE *pwType,
								   UINT *puMaxLen, BOOL *pbFixLen)
{
	ATLASSERT(lpTypeStr && pwType && puMaxLen && pbFixLen);
	BOOL bResult = TRUE;
	UINT cTokens = 0;
	char rgTokens[MYSQL_METANAMELEN + 1][16];
	// split string into tokens
	char *lpEnd;
	char *lpStart = lpTypeStr;
	do
	{
		lpEnd = strchr(lpStart, ' ');
		if (lpEnd)
		{
			lstrcpynA(rgTokens[cTokens++], lpStart, lpEnd - lpStart + 1);
			lpStart = lpEnd + 1;
		}
		else
			lstrcpynA(rgTokens[cTokens++], lpStart, lpTypeStr + lstrlenA(lpTypeStr) - lpStart + 1);

	} while (lpEnd);

	BOOL bUnsigned = FALSE;
	for (UINT i = 1; i < cTokens; i++)
	{
		if (!strcmp(rgTokens[i], "unsigned"))
			bUnsigned = TRUE;
	}

	char lpType[MYSQL_METANAMELEN + 1];
	UINT uMaxLen = 0;
	int cRead = sscanf(rgTokens[0], "%[a-z](%u)", lpType, &uMaxLen);
	if (cRead == 0)
		return FALSE;

	DBTYPE wType;
	BOOL bFixLen = TRUE;
	if (!strcmp(lpType, "int"))
	{
		wType = bUnsigned ? DBTYPE_UI4 : DBTYPE_I4;
		*puMaxLen = 0;
	}
	else if (!strcmp(lpType, "smallint"))
	{
		wType = bUnsigned ? DBTYPE_UI2 : DBTYPE_I2;
		*puMaxLen = 0;
	}
	else if (!strcmp(lpType, "tinyint"))
	{
		wType = bUnsigned ? DBTYPE_UI1 : DBTYPE_I1;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "mediumint"))
	{
		wType = bUnsigned ? DBTYPE_UI4 : DBTYPE_I4;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "bigint"))
	{
		wType = bUnsigned ? DBTYPE_UI8 : DBTYPE_I8;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "char") || !strcmp(lpType, "varchar"))
	{
		wType = DBTYPE_STR;
		bFixLen = FALSE;
		//uMaxLen = 255;
	}
	else if (!strcmp(lpType, "tinytext"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 255;
	}
	else if (!strcmp(lpType, "text"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 65535;
	}
	else if (!strcmp(lpType, "mediumtext"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 16777215;
	}
	else if (!strcmp(lpType, "longtext"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 4294967295;
	}
	else if (!strcmp(lpType, "tinyblob"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 255;
	}
	else if (!strcmp(lpType, "blob"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 65535;
	}
	else if (!strcmp(lpType, "mediumblob"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 16777215;
	}
	else if (!strcmp(lpType, "longblob"))
	{
		wType = DBTYPE_BYTES;
		bFixLen = FALSE;
		uMaxLen = 4294967295;
	}
	else if (!strcmp(lpType, "date"))
	{
		wType = DBTYPE_DBDATE;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "datetime"))
	{
		wType = DBTYPE_DBTIMESTAMP;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "timestamp"))
	{
		wType = DBTYPE_DBTIMESTAMP;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "time"))
	{
		wType = DBTYPE_DBTIME;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "year"))
	{
		wType = DBTYPE_UI2;
		uMaxLen = 0;
	}
	else if (!strcmp(lpType, "set"))
	{
		wType = DBTYPE_STR;
		bFixLen = FALSE;
		uMaxLen = 255;
	}
	else if (!strcmp(lpType, "enum"))
	{
		wType = DBTYPE_STR;
		bFixLen = FALSE;
		uMaxLen = 255;
	}
	else
	{
		bResult = FALSE;
	}

	*pwType = wType;
	*puMaxLen = uMaxLen;
	*pbFixLen = bFixLen;
	return bResult;
}	

/////////////////////////////////////////////////////////////////////////

BOOL CMetaEnumerator::ParseDefaultStr(char *lpDefaultStr, BOOL *pbHasDefault, LPOLESTR szDefault)
{
	USES_CONVERSION;

	if (strlen(lpDefaultStr) == 0)
	{
		*pbHasDefault = FALSE;
		szDefault[0] = 0;
	}
	else
	{
		*pbHasDefault = TRUE;
		if (!strcmpi(lpDefaultStr, "NULL"))
			szDefault[0] = 0;
		else
			wcscpy(szDefault, A2OLE(lpDefaultStr));
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////

BOOL CMetaEnumerator::ParseNullStr(char *lpNullStr, BOOL *pbNull)
{
	*pbNull = !strcmp(lpNullStr, "YES");
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////

BOOL CMetaEnumerator::ParseKeyStr(char *lpKeyStr, BOOL *pbPrimary, BOOL *pbForeign)
{
	*pbPrimary = !strcmp(lpKeyStr, "PRI");
	*pbForeign = FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////