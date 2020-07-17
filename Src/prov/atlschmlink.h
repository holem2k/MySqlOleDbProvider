#ifndef __ATLSCHMLINK_H__
#define __ATLSCHMLINK_H__
#include "atldbfix.h"
#include "lab\myapi\myobjects.h"

const ULONG MYSQL_METANAMELEN = 64;
const ULONG MYSQL_METADEFAULTLEN = 255;

struct PROVIDERTYPE_DATA
{
	WCHAR			*m_szName;		// 1
	USHORT          m_nType;			// 2
	ULONG           m_ulSize;			// 3
	WCHAR           *m_szPrefix;	//  4
	WCHAR           *m_szSuffix;	// 5
	WCHAR           *m_szCreateParams;	// 6
	VARIANT_BOOL    m_bIsNullable;		// 7
	VARIANT_BOOL    m_bCaseSensitive;	// 8
	ULONG           m_bSearchable;		// 9
	VARIANT_BOOL    m_bUnsignedAttribute;	// 10
	VARIANT_BOOL    m_bFixedPrecScale;	// 11
	VARIANT_BOOL    m_bAutoUniqueValue;	// 12
	WCHAR           *m_szLocalTypeName;	// 13
	short           m_nMinScale;		// 14
	short           m_nMaxScale;		// 15
	GUID            m_guidType;			// 16
	WCHAR			*m_szTypeLib;	// 17
	WCHAR           *m_szVersion;	// 18
	VARIANT_BOOL    m_bIsLong;			// 19
	VARIANT_BOOL    m_bBestMatch;		// 20
	VARIANT_BOOL	m_bIsFixedLength;	// 21
}; 

void CopyProviderTypeData(CPROVIDER_TYPERowEx *pRow, const PROVIDERTYPE_DATA *pData);

struct DATABASEINFO
{
	OLECHAR szDatabase[MYSQL_METANAMELEN + 1];
};

struct TABLEINFO
{
	OLECHAR szTable[MYSQL_METANAMELEN + 1];
};

struct SCHEMA_COLUMNINFO
{
	UINT uOrdinal;
	OLECHAR szColumn[MYSQL_METANAMELEN + 1];
	DBTYPE wType;
	BOOL bFixLen;
	UINT uMaxLen;
	BOOL bHasDefault;
	OLECHAR szDefault[MYSQL_METADEFAULTLEN + 1];
	BOOL bIsNullable;
	BOOL bPrimary;
	BOOL bForeign;
};

class CMetaEnumerator 
{
public:
	BOOL EnumDatabases(CMySqlConnection *pConnection, UINT *pcDatabases, DATABASEINFO **ppInfo);
	BOOL EnumTables(CMySqlConnection *pConnection, LPOLESTR szDatabase, UINT *pcTables, TABLEINFO **ppInfo);
	BOOL EnumColumns(CMySqlConnection *pConnection, LPOLESTR szDatabase, LPOLESTR szTable,
		UINT *pcColumns,  SCHEMA_COLUMNINFO **ppInfo);
protected:
	BOOL ReadRawRow(CMySqlResult *pResult, UINT nRow,
		char *lpColumnName, char *lpColumnType,
		char *lpColumnNull, char *lpColumnKey,
		char *lpColumnDefault, char *lpColumnExtra);
	BOOL ConvertRawRow(char *lpColumnName, char *lpColumnType,
		char *lpColumnNull, char *lpColumnKey,
		char *lpColumnDefault, char *lpColumnExtra,
		SCHEMA_COLUMNINFO *pInfo, UINT nRow);
	BOOL ParseTypeStr(char *lpTypeStr, DBTYPE *pwType, UINT *puMaxLen, BOOL *pbFixLen);
	BOOL ParseDefaultStr(char *lpDefaultStr, BOOL *pbHasDefault, LPOLESTR szDefault);
	BOOL ParseNullStr(char *lpNullStr, BOOL *pbNull);
	BOOL ParseKeyStr(char *lpKeyStr, BOOL *pbPrimary, BOOL *pbForeign);
};




#endif //__ATLSCHMLINK_H__