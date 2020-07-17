#include "stdafx.h"
#include "MySqlSess.h"
#include "atlschmlink.h"

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionCatalogsRowset

HRESULT CMySqlSessionCatalogsRowset::Execute(LONG* pcRowsAffected, ULONG cRestrictions, const VARIANT *rgRestrictions)
{
	CComPtr<IConnectionSource> spConnSrc;
	if (FAILED(GetConnectionSource(spConnSrc)))
		return E_FAIL;

	// assume failure
	HRESULT hr = E_FAIL;

	ULONG cRowsAffected = 0;
	CMySqlConnection *pConnection;
	if (SUCCEEDED(spConnSrc->GetConnection((void **)&pConnection)))
	{
		CMetaEnumerator enumerator;
		UINT cDatabases;
		DATABASEINFO *pInfo;
		
		BOOL bEnum = enumerator.EnumDatabases(pConnection, &cDatabases, &pInfo);
		if (bEnum)
		{
			for (UINT i = 0; i < cDatabases; i++)
			{
				if (!CheckRestriction_WSTR(pInfo[i].szDatabase, rgRestrictions, 0))
					continue;
				
				CCATALOGS_Row row;
				wcsncpy(row.m_szCatalogName,  pInfo[i].szDatabase, sizeof(row.m_szCatalogName)/sizeof(row.m_szCatalogName[0]));
				m_rgRowData.Add(row);
				cRowsAffected++;
			}
			CoTaskMemFree(pInfo);
			hr = S_OK;
		}
		spConnSrc->ReleaseConnection(pConnection);
	}

	if (pcRowsAffected != NULL)
		*pcRowsAffected = cRowsAffected;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////

DBSTATUS CMySqlSessionCatalogsRowset::GetDBStatus(CSimpleRow*, ATLCOLUMNINFO* pInfo)
{
	if (pInfo->iOrdinal == 2)
		return DBSTATUS_S_ISNULL;
	return DBSTATUS_S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionTRSchemaRowset

HRESULT CMySqlSessionTRSchemaRowset::Execute(LONG *pcRowsAffected, ULONG cRestrictions, const VARIANT *rgRestrictions)
{
	CComPtr<IConnectionSource> spConnSrc;
	if (FAILED(GetConnectionSource(spConnSrc)))
		return E_FAIL;

	// assume failure
	HRESULT hr = E_FAIL;
	LONG cRowsAffected = 0;
	CMySqlConnection *pConnection;
	if (SUCCEEDED(spConnSrc->GetConnection((void **)&pConnection)))
	{
		CMetaEnumerator enumerator;
		UINT cDatabases;
		DATABASEINFO *pInfo;

		BOOL bDbEnum = enumerator.EnumDatabases(pConnection, &cDatabases, &pInfo);
		if (bDbEnum)
		{
			for (UINT i = 0; i < cDatabases; i++)
			{
				if (!CheckRestriction_WSTR(pInfo[i].szDatabase, rgRestrictions, 0))
					continue;

				UINT cTables;
				TABLEINFO *pTblInfo;
				BOOL bTblEnum = enumerator.EnumTables(pConnection, pInfo[i].szDatabase,
					&cTables, &pTblInfo);
				if (bTblEnum)
				{
					for (UINT j = 0; j < cTables; j++)
					{
						if (!CheckRestriction_WSTR(pTblInfo[j].szTable, rgRestrictions, 2))
							continue;

						CTABLESRowEx row;
						if (!wcsicmp(pInfo[i].szDatabase, OLESTR("mysql")))
							wcscpy(row.m_szType, OLESTR("SYSTEM TABLE"));
						else
							wcscpy(row.m_szType, OLESTR("TABLE"));
						wcscpy(row.m_szCatalog, pInfo[i].szDatabase);
						wcscpy(row.m_szDesc, OLESTR("MySQL Table"));
						wcscpy(row.m_szTable, pTblInfo[j].szTable);
						if (CheckRestriction_WSTR(row.m_szType, rgRestrictions, 3))
						{
							m_rgRowData.Add(row);
							cRowsAffected++;
						}
					}
					CoTaskMemFree(pTblInfo);
					hr = S_OK;
				}
				else
					break;
			}
			CoTaskMemFree(pInfo);
		}
		spConnSrc->ReleaseConnection(pConnection);
	}

	if (pcRowsAffected)
		*pcRowsAffected = cRowsAffected; // 0 if failure
	return hr;
}

/////////////////////////////////////////////////////////////////////////////

DBSTATUS CMySqlSessionTRSchemaRowset::GetDBStatus(CSimpleRow*, ATLCOLUMNINFO* pInfo)
{
	if (pInfo->iOrdinal == 2/*TABLE_SCHEME*/ ||
		pInfo->iOrdinal == 5/*TABLE_GUID*/ ||
		pInfo->iOrdinal == 7/*TABLE_PROPID*/)
		return DBSTATUS_S_ISNULL;
	return DBSTATUS_S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionColSchemaRowset

HRESULT CMySqlSessionColSchemaRowset::Execute(LONG *pcRowsAffected, ULONG cRestrictions, const VARIANT *rgRestrictions)
{
	CComPtr<IConnectionSource> spConnSrc;
	if (FAILED(GetConnectionSource(spConnSrc)))
		return E_FAIL;

	HRESULT hr = E_FAIL;

	UINT cRowsAffected = 0;
	CMySqlConnection *pConnection;
	if (SUCCEEDED(spConnSrc->GetConnection((void **)&pConnection)))
	{
		CMetaEnumerator enumerator;
		UINT cDatabases;
		DATABASEINFO *pInfo;
		BOOL bDbEnum = enumerator.EnumDatabases(pConnection, &cDatabases, &pInfo);
		if (bDbEnum)
		{
			for (UINT i = 0; i < cDatabases; i++)
			{
				if (!CheckRestriction_WSTR(pInfo[i].szDatabase, rgRestrictions, 0))
					continue;

				UINT cTables;
				TABLEINFO *pTblInfo;
				BOOL bTblEnum = enumerator.EnumTables(pConnection, pInfo[i].szDatabase,
					&cTables, &pTblInfo);
				if (bTblEnum)
				{
					for (UINT j = 0; j < cTables; j++)
					{
						if (!CheckRestriction_WSTR(pTblInfo[j].szTable, rgRestrictions, 2))
							continue;

						UINT cColumns;
						SCHEMA_COLUMNINFO *pColInfo;
						BOOL bColEnum = enumerator.EnumColumns(pConnection,
							pInfo[i].szDatabase, pTblInfo[j].szTable, &cColumns, &pColInfo);
						if (bColEnum)
						{
							for (UINT l = 0; l < cColumns; l++)
							{
								if (!CheckRestriction_WSTR(pColInfo[l].szColumn, rgRestrictions, 3))
									continue;

								CCOLUMNSRowEx row;
								// TO DO
								wcscpy(row.m_szTableCatalog, pInfo[i].szDatabase);
								wcscpy(row.m_szTableName, pTblInfo[j].szTable);
								wcscpy(row.m_szColumnName, pColInfo[l].szColumn);
								row.m_ulOrdinalPosition = pColInfo[l].uOrdinal;
								row.m_nDataType = pColInfo[l].wType;
								row.m_ulCharMaxLength = pColInfo[l].uMaxLen;	
								row.m_ulCharOctetLength = pColInfo[l].uMaxLen;
								if (pColInfo[l].wType == DBTYPE_WSTR)
									row.m_ulCharOctetLength *= sizeof(OLECHAR);
								row.m_bColumnHasDefault = pColInfo[l].bHasDefault;
								wcscpy(row.m_szColumnDefault, pColInfo[l].szDefault);
								row.m_ulColumnFlags = 0;
								if (pColInfo[l].bFixLen)
									row.m_ulColumnFlags |= DBCOLUMNFLAGS_ISFIXEDLENGTH;
								if (pColInfo[l].bIsNullable)
									row.m_ulColumnFlags |= DBCOLUMNFLAGS_ISNULLABLE | DBCOLUMNFLAGS_MAYBENULL;
								row.m_bIsNullable = pColInfo[l].bIsNullable;
								row.m_nNumericPrecision = GetTypePrecision(pColInfo[i].wType);
								m_rgRowData.Add(row);
								cRowsAffected++;
							}
							CoTaskMemFree(pColInfo);
							hr = S_OK;
						}
					}
					CoTaskMemFree(pTblInfo);
				}
			}
			CoTaskMemFree(pInfo);
		}
		spConnSrc->ReleaseConnection(pConnection);
	}

	if (pcRowsAffected)
		*pcRowsAffected = cRowsAffected;
	return hr;
}

/////////////////////////////////////////////////////////////////////////////

DBSTATUS CMySqlSessionColSchemaRowset::GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO* pInfo)
{
	DBSTATUS dbStatus = DBSTATUS_S_OK;
	switch(pInfo->iOrdinal)
	{
	case 2:  // TABLE_SCHEMA
	case 5:  // COLUMN_GUID
	case 6:  // COLUMN_PROPID
	case 13: // TYPE_GUID
	case 19: // CHARACTER_SET_CATALOG
	case 20: // CHARACTER_SET_SCHEMA
	case 21: // CHARACTER_SET_NAME
	case 22: // COLLATION_CATALOG
	case 23: // COLLATION_SCHEMA
	case 24: // COLLATION_NAME
	case 25: // DOMAIN_CATALOG
	case 26: // DOMAIN_SCHEMA
	case 27: // DOMAIN_NAME
	case 28: // DESCRIPTION
		dbStatus = DBSTATUS_S_ISNULL;
		break;

	case 14: // CHARACTER_MAXIMUM_LENGTH
	case 15: // CHARACTER_OCTET_LENGTH
		{
			DBTYPE wType = m_rgRowData[pRow->m_iRowset].m_nDataType;
			if (wType != DBTYPE_STR &&
				wType != DBTYPE_WSTR &&
				wType != DBTYPE_BYTES)
				dbStatus = DBSTATUS_S_ISNULL;
		}
		break;

	case 16: // NUMERIC_PRECISION
		{
			DBTYPE wType = m_rgRowData[pRow->m_iRowset].m_nDataType;
			if (wType != DBTYPE_I1 && wType != DBTYPE_UI1 &&
				wType != DBTYPE_I2 && wType != DBTYPE_UI2 &&
				wType != DBTYPE_I4 && wType != DBTYPE_UI4 &&
				wType != DBTYPE_I8 && wType != DBTYPE_UI8 &&
				wType != DBTYPE_R4 && wType != DBTYPE_R8)
				dbStatus = DBSTATUS_S_ISNULL;
		}
		break;

	case 17: // NUMERIC_SCALE
		{
			DBTYPE wType = m_rgRowData[pRow->m_iRowset].m_nDataType;
			if (wType != DBTYPE_DECIMAL &&
				wType != DBTYPE_NUMERIC)
				dbStatus = DBSTATUS_S_ISNULL;
		}
		break;

	case 18: // DATETIME_PRECISION
		{
			DBTYPE wType = m_rgRowData[pRow->m_iRowset].m_nDataType;
			if (wType != DBTYPE_DBTIME &&
				wType != DBTYPE_DBDATE &&
				wType != DBTYPE_DBTIMESTAMP)
				dbStatus = DBSTATUS_S_ISNULL;
		}
		break;

	case 9: // COLUMN_DEFAULT
		{
			if (wcslen(m_rgRowData[pRow->m_iRowset].m_szColumnDefault) == 0)
				dbStatus = DBSTATUS_S_ISNULL;
		}
		break;
	}
	return dbStatus;
}

/////////////////////////////////////////////////////////////////////////////

BYTE CMySqlSessionColSchemaRowset::GetTypePrecision(DBTYPE wType)
{
	BYTE bPres = ~0;
	switch (wType)
	{
	case DBTYPE_UI1:
	case DBTYPE_I1:
		bPres = 3;
		break;

	case DBTYPE_I2:
	case DBTYPE_UI2:
		bPres = 5;
		break;

	case DBTYPE_I4:
	case DBTYPE_UI4:
		bPres = 10;
		break;

	case DBTYPE_I8:
		bPres = 19;
		break;

	case DBTYPE_UI8:
		bPres = 20;
		break;

	case DBTYPE_R4:
		bPres = 7;
		break;

	case DBTYPE_R8:
		bPres = 16;
		break;

	case DBTYPE_CY:
		bPres = 19;
		break;

	case DBTYPE_NUMERIC:
		bPres = 38;
		break;

	case DBTYPE_DECIMAL:
		bPres = 28;
		break;
	}
	return bPres;
}

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionPTSchemaRowset

const PROVIDERTYPE_DATA rgProviderTypes[] = 
{
	// int
	{
		OLESTR("int"),	// m_szName
		DBTYPE_I4,		// m_nType
		10,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("int"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),	// m_szTypeLib
		OLESTR(""),	// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// unsigned int
	{
		OLESTR("int unsigned"),	// m_szName
		DBTYPE_UI4,		// m_nType
		10,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_TRUE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("int unsigned"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),	// m_szTypeLib
		OLESTR(""),	// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// tinyint
	{
		OLESTR("tinyint"),	// m_szName
		DBTYPE_I1,		// m_nType
		3,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("tinyint"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),	// m_szTypeLib
		OLESTR(""),	// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// unsigned tinyint
	{
		OLESTR("tinyint unsigned"),	// m_szName
		DBTYPE_UI1,		// m_nType
		3,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_TRUE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("tinyint unsigned"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),	// m_szTypeLib
		OLESTR(""),	// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// mediumint
	{
		OLESTR("mediumint"),	// m_szName
		DBTYPE_I4,				// m_nType
		8,						// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("mediumint"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// unsigned mediumint
	{
		OLESTR("mediumint unsigned"),	// m_szName
		DBTYPE_UI4,		// m_nType
		8,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_TRUE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("mediumint unsigned"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),	// m_szTypeLib
		OLESTR(""),	// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// smallint
	{
		OLESTR("smallint"),	// m_szName
		DBTYPE_I2,				// m_nType
		5,						// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("smallint"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// unsigned mediumint
	{
		OLESTR("smallint unsigned"),	// m_szName
		DBTYPE_UI4,		// m_nType
		5,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_TRUE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("smallint unsigned"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),	// m_szTypeLib
		OLESTR(""),	// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// bigint
	{
		OLESTR("bigint"),	// m_szName
		DBTYPE_I8,				// m_nType
		20,						// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("smallint"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// unsigned bigint
	{
		OLESTR("bigint unsigned"),	// m_szName
		DBTYPE_UI8,		// m_nType
		20,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_TRUE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("bigint unsigned"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),	// m_szTypeLib
		OLESTR(""),	// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// float
	{
		OLESTR("float"),	// m_szName
		DBTYPE_R4,				// m_nType
		7,						// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("float"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// double
	{
		OLESTR("double"),	// m_szName
		DBTYPE_R8,				// m_nType
		15,						// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("double"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// decimal
	{
		OLESTR("decimal"),	// m_szName
		DBTYPE_R8,				// m_nType
		15,						// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR("precision,scale"),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("decimal"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// char
	{
		OLESTR("char"),	// m_szName
		DBTYPE_STR,		// m_nType
		255,			// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR("length"),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("char"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// varchar
	{
		OLESTR("varchar"),	// m_szName
		DBTYPE_STR,				// m_nType
		255,						// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR("length"),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("varchar"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// text
	{
		OLESTR("text"),	// m_szName
		DBTYPE_STR,		// m_nType
		65535,			// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("text"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_TRUE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// longtext (CURRENTLY UNSUPPORTED) 
	/*
	{
		OLESTR("longtext"),	// m_szName
		DBTYPE_STR,		// m_nType
		4294967295,		// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("longtext"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_TRUE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	*/
	// mediumtext 
	{
		OLESTR("mediumtext"),	// m_szName
		DBTYPE_STR,		// m_nType
		16777215,		// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("mediumtext"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_TRUE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// tinyblob
	{
		OLESTR("tinyblob"),	// m_szName
		DBTYPE_STR,			// m_nType
		255,				// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("tinyblob"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// blob
	{
		OLESTR("blob"),	// m_szName
		DBTYPE_STR,			// m_nType
		65535,				// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("blob"),	// m_szLocalTypeName
	    0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_TRUE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// mediumblob
	{
		OLESTR("mediumblob"),	// m_szName
		DBTYPE_BYTES,		// m_nType
		16777215,		// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("mediumblob"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_TRUE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// longblob (CURRENTLY UNSUPPORTED) 
	/*
	{
		OLESTR("longblob"),	// m_szName
		DBTYPE_BYTES,		// m_nType
		4294967295,		// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("longblob"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_TRUE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	*/
	// enum
	{
		OLESTR("enum"),	// m_szName
		DBTYPE_STR,		// m_nType
		255,			// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR("value1,value2,..."),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("enum"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// set
	{
		OLESTR("set"),	// m_szName
		DBTYPE_STR,		// m_nType
		255,			// m_ulSize
		OLESTR("'"),	// m_szPrefix
		OLESTR("'"),	// m_szSuffix
		OLESTR("value1,value2,..."),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("set"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_FALSE	// m_bIsFixedLength
	},
	// year
	{
		OLESTR("year"),	// m_szName
		DBTYPE_UI2,		// m_nType
		4,				// m_ulSize
		OLESTR(""),	// m_szPrefix
		OLESTR(""),	// m_szSuffix
		OLESTR(""),	// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_TRUE,	// m_bUnsignedAttribute
		VARIANT_TRUE,   // m_bFixedPrecScale
		VARIANT_FALSE,  // m_bAutoUniqueValue
		OLESTR("year"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// timestamp
	{
		OLESTR("timestamp"),	// m_szName
		DBTYPE_DBTIMESTAMP,		// m_nType
		14,						// m_ulSize
		OLESTR(""),		// m_szPrefix
		OLESTR(""),		// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,	// m_bFixedPrecScale
		VARIANT_FALSE,	// m_bAutoUniqueValue
		OLESTR("timestamp"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_FALSE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// datetime
	{
		OLESTR("datetime"),		// m_szName
		DBTYPE_DBTIMESTAMP,		// m_nType
		21,						// m_ulSize
		OLESTR(""),		// m_szPrefix
		OLESTR(""),		// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,	// m_bFixedPrecScale
		VARIANT_FALSE,	// m_bAutoUniqueValue
		OLESTR("datetime"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// time
	{
		OLESTR("time"),		// m_szName
		DBTYPE_DBTIME,		// m_nType
		6,					// m_ulSize
		OLESTR(""),		// m_szPrefix
		OLESTR(""),		// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,	// m_bFixedPrecScale
		VARIANT_FALSE,	// m_bAutoUniqueValue
		OLESTR("time"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},
	// date
	{
		OLESTR("date"),		// m_szName
		DBTYPE_DBTIME,		// m_nType
		10,					// m_ulSize
		OLESTR(""),		// m_szPrefix
		OLESTR(""),		// m_szSuffix
		OLESTR(""),		// m_szCreateParams
		VARIANT_TRUE,   // m_bIsNullable
		VARIANT_FALSE,	// m_bCaseSensitive
		DB_SEARCHABLE,	// m_bSearchable
		VARIANT_FALSE,	// m_bUnsignedAttribute
		VARIANT_FALSE,	// m_bFixedPrecScale
		VARIANT_FALSE,	// m_bAutoUniqueValue
		OLESTR("date"),	// m_szLocalTypeName
		0,				// m_nMinScale
		0,				// m_nMaxScale
		{0,0,0,{0,0,0,0,0,0,0,0}},		// m_guidType
		OLESTR(""),		// m_szTypeLib
		OLESTR(""),		// m_szVersion
		VARIANT_FALSE,	// m_bIsLong
		VARIANT_TRUE,	// m_bBestMatch
		VARIANT_TRUE	// m_bIsFixedLength
	},

};

HRESULT CMySqlSessionPTSchemaRowset::Execute(LONG* pcRowsAffected, ULONG, const VARIANT*)
{
	*pcRowsAffected = 0;
	CPROVIDER_TYPERowEx row;
	for (int i = 0; i < sizeof(rgProviderTypes)/sizeof(rgProviderTypes[0]); i++)
	{
		CopyProviderTypeData(&row, rgProviderTypes + i);
		m_rgRowData.Add(row);
		*pcRowsAffected++;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

DBSTATUS CMySqlSessionPTSchemaRowset::GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO* pInfo)
{
	DBSTATUS dbStatus = DBSTATUS_S_OK;
	switch (pInfo->iOrdinal)
	{
	case 4:
		if (!m_rgRowData[pRow->m_iRowset].m_szPrefix[0])
			dbStatus = DBSTATUS_S_ISNULL;
		break;

	case 5:
		if (!m_rgRowData[pRow->m_iRowset].m_szSuffix[0])
			dbStatus = DBSTATUS_S_ISNULL;
		break;

	case 6:
		if (!m_rgRowData[pRow->m_iRowset].m_szCreateParams[0])
			dbStatus = DBSTATUS_S_ISNULL;
		break;

	case 10: //
		{
			DBTYPE wType = m_rgRowData[pRow->m_iRowset].m_nType;
			if (wType != DBTYPE_I1 &&
				wType != DBTYPE_UI1 &&
				wType != DBTYPE_I2 &&
				wType != DBTYPE_UI2 &&
				wType != DBTYPE_I4 &&
				wType != DBTYPE_UI4 &&
				wType != DBTYPE_R4 &&
				wType != DBTYPE_R8)
				dbStatus = DBSTATUS_S_ISNULL;
		}
		break;

	case 13:
		if (!m_rgRowData[pRow->m_iRowset].m_szLocalTypeName[0])
			dbStatus = DBSTATUS_S_ISNULL;
		break;

	case 14:
	case 15: // UNSIGNED_ATTRIBUTE
		{
			DBTYPE wType = m_rgRowData[pRow->m_iRowset].m_nType;
			if (wType != DBTYPE_DECIMAL && wType != DBTYPE_NUMERIC)
				dbStatus = DBSTATUS_S_ISNULL;
		}
			break;

		// always NULL
	case 16: // GUID
	case 17: // TYPELIB
	case 18: // VERSION
		dbStatus = DBSTATUS_S_ISNULL;
		break;
	}
	return dbStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionPKSchemaRowset 

HRESULT CMySqlSessionPKSchemaRowset::Execute(LONG *pcRowsAffected, ULONG cRestrictions, 
											 const VARIANT *rgRestrictions)
{
	CComPtr<IConnectionSource> spConnSrc;
	if (FAILED(GetConnectionSource(spConnSrc)))
		return E_FAIL;

	HRESULT hr = E_FAIL;

	UINT cRowsAffected = 0;
	CMySqlConnection *pConnection;
	if (SUCCEEDED(spConnSrc->GetConnection((void **)&pConnection)))
	{
		CMetaEnumerator enumerator;
		UINT cDatabases;
		DATABASEINFO *pInfo;
		BOOL bDbEnum = enumerator.EnumDatabases(pConnection, &cDatabases, &pInfo);
		if (bDbEnum)
		{
			for (UINT i = 0; i < cDatabases; i++)
			{
				if (!CheckRestriction_WSTR(pInfo[i].szDatabase, rgRestrictions, 0))
					continue;

				UINT cTables;
				TABLEINFO *pTblInfo;
				BOOL bTblEnum = enumerator.EnumTables(pConnection, pInfo[i].szDatabase,
					&cTables, &pTblInfo);
				if (bTblEnum)
				{
					for (UINT j = 0; j < cTables; j++)
					{
						if (!CheckRestriction_WSTR(pTblInfo[j].szTable, rgRestrictions, 2))
							continue;

						UINT cColumns;
						SCHEMA_COLUMNINFO *pColInfo;
						BOOL bColEnum = enumerator.EnumColumns(pConnection,
							pInfo[i].szDatabase, pTblInfo[j].szTable, &cColumns, &pColInfo);
						if (bColEnum)
						{
							BOOL nKeyOrdinal = 1;
							for (UINT l = 0; l < cColumns; l++)
							{
								if (pColInfo[l].bPrimary)
								{
									CPRIMARY_KEYRow row;
									row.m_nOrdinal = nKeyOrdinal++;
									wcscpy(row.m_szTableCatalog, pInfo[i].szDatabase);
									wcscpy(row.m_szTableName, pTblInfo[j].szTable);
									wcscpy(row.m_szColumnName, pColInfo[l].szColumn);
									wcscpy(row.m_szPkName, OLESTR("Primary key"));
									m_rgRowData.Add(row);
									cRowsAffected++;
								}
							}
							CoTaskMemFree(pColInfo);
							hr = S_OK;
						}
					}
					CoTaskMemFree(pTblInfo);
				}
			}
			CoTaskMemFree(pInfo);
		}
		spConnSrc->ReleaseConnection(pConnection);
	}

	if (pcRowsAffected)
		*pcRowsAffected = cRowsAffected;
	return hr;
}

/////////////////////////////////////////////////////////////////////////////

DBSTATUS CMySqlSessionPKSchemaRowset::GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pInfo)
{
	DBSTATUS dbStatus = DBSTATUS_S_OK;
	switch (pInfo->iOrdinal)
	{
	case 2: // TABLE_SCHEMA
	case 5: // COLUMN_GUID
	case 6: // COLUMN_PROPID
//	case 8: // PK_NAME
		dbStatus = DBSTATUS_S_ISNULL;
		break;
	}
	return dbStatus;
}

/////////////////////////////////////////////////////////////////////////////
