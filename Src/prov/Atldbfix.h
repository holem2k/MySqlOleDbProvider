#ifndef __ATLDBFIX_H__
#define __ATLDBFIX_H__

// code snippet from atldb.h with fixed column map

class CPROVIDER_TYPERowEx
{
public:
// Attributes
	WCHAR           m_szName[129];
	USHORT          m_nType;
	ULONG           m_ulSize;
	WCHAR           m_szPrefix[129];
	WCHAR           m_szSuffix[129];
	WCHAR           m_szCreateParams[129];
	VARIANT_BOOL    m_bIsNullable;
	VARIANT_BOOL    m_bCaseSensitive;
	ULONG           m_bSearchable;
	VARIANT_BOOL    m_bUnsignedAttribute;
	VARIANT_BOOL    m_bFixedPrecScale;
	VARIANT_BOOL    m_bAutoUniqueValue;
	WCHAR           m_szLocalTypeName[129];
	short           m_nMinScale;
	short           m_nMaxScale;
	GUID            m_guidType;
	WCHAR           m_szTypeLib[129];
	WCHAR           m_szVersion[129];
	VARIANT_BOOL    m_bIsLong;
	VARIANT_BOOL    m_bBestMatch;
	VARIANT_BOOL	m_bIsFixedLength;

	CPROVIDER_TYPERowEx()
	{
		m_szName[0] = NULL;
		m_nType = 0;
		m_ulSize = 0;
		m_szPrefix[0] = NULL;
		m_szSuffix[0] = NULL;
		m_szCreateParams[0] = NULL;
		m_bIsNullable = VARIANT_FALSE;
		m_bCaseSensitive = VARIANT_FALSE;
		m_bSearchable = DB_UNSEARCHABLE;
		m_bUnsignedAttribute = VARIANT_FALSE;
		m_bFixedPrecScale = VARIANT_FALSE;
		m_bAutoUniqueValue = VARIANT_FALSE;
		m_szLocalTypeName[0] = NULL;
		m_nMinScale = 0;
		m_nMaxScale = 0;
		m_guidType = GUID_NULL;
		m_szTypeLib[0] = NULL;
		m_szVersion[0] = NULL;
		m_bIsLong = VARIANT_FALSE;
		m_bBestMatch = VARIANT_FALSE;
		m_bIsFixedLength = VARIANT_FALSE;
	}
// Binding Maps
BEGIN_PROVIDER_COLUMN_MAP(CPROVIDER_TYPERowEx)
	PROVIDER_COLUMN_ENTRY_WSTR("TYPE_NAME", 1, m_szName)
	PROVIDER_COLUMN_ENTRY("DATA_TYPE", 2, m_nType)
	PROVIDER_COLUMN_ENTRY("COLUMN_SIZE", 3, m_ulSize)
	PROVIDER_COLUMN_ENTRY_WSTR("LITERAL_PREFIX", 4, m_szPrefix)
	PROVIDER_COLUMN_ENTRY_WSTR("LITERAL_SUFFIX", 5, m_szSuffix)
	PROVIDER_COLUMN_ENTRY_WSTR("CREATE_PARAMS", 6, m_szCreateParams)
	PROVIDER_COLUMN_ENTRY_FIXED("IS_NULLABLE", 7, DBTYPE_BOOL, m_bIsNullable)
	PROVIDER_COLUMN_ENTRY_FIXED("CASE_SENSITIVE", 8,  DBTYPE_BOOL, m_bCaseSensitive)
	PROVIDER_COLUMN_ENTRY_FIXED("SEARCHABLE", 9,  DBTYPE_UI4, m_bSearchable)
	PROVIDER_COLUMN_ENTRY_FIXED("UNSIGNED_ATTRIBUTE", 10,  DBTYPE_BOOL, m_bUnsignedAttribute)
	PROVIDER_COLUMN_ENTRY_FIXED("FIXED_PREC_SCALE", 11,  DBTYPE_BOOL, m_bFixedPrecScale)
	PROVIDER_COLUMN_ENTRY_FIXED("AUTO_UNIQUE_VALUE", 12,  DBTYPE_BOOL, m_bAutoUniqueValue)
	PROVIDER_COLUMN_ENTRY_WSTR("LOCAL_TYPE_NAME", 13, m_szLocalTypeName)
	PROVIDER_COLUMN_ENTRY("MINIMUM_SCALE", 14, m_nMinScale)
	PROVIDER_COLUMN_ENTRY("MAXIMUM_SCALE", 15, m_nMaxScale)
	PROVIDER_COLUMN_ENTRY("GUID", 16, m_guidType)
	PROVIDER_COLUMN_ENTRY_WSTR("TYPELIB", 17, m_szTypeLib)
	PROVIDER_COLUMN_ENTRY_WSTR("VERSION", 18, m_szVersion)
	PROVIDER_COLUMN_ENTRY_FIXED("IS_LONG", 19, DBTYPE_BOOL, m_bIsLong)
	PROVIDER_COLUMN_ENTRY_FIXED("BEST_MATCH", 20, DBTYPE_BOOL, m_bBestMatch)
	PROVIDER_COLUMN_ENTRY_FIXED("IS_FIXEDLENGTH", 21, DBTYPE_BOOL, m_bIsFixedLength)
END_PROVIDER_COLUMN_MAP()
};


class CTABLESRowEx
{
public:
	WCHAR m_szCatalog[129];
	WCHAR m_szSchema[129];
	WCHAR m_szTable[129];
	WCHAR m_szType[129];
	WCHAR m_szDesc[129];
	GUID  m_guid;
	ULONG m_ulPropID;

	CTABLESRowEx()
	{
		m_szCatalog[0] = NULL;
		m_szSchema[0] = NULL;
		m_szTable[0] = NULL;
		m_szType[0] = NULL;
		m_szDesc[0] = NULL;
		m_guid = GUID_NULL;
		m_ulPropID = 0;
	}

BEGIN_PROVIDER_COLUMN_MAP(CTABLESRowEx)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_CATALOG", 1, m_szCatalog)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_SCHEMA", 2, m_szSchema)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_NAME", 3, m_szTable)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_TYPE", 4, m_szType)
	PROVIDER_COLUMN_ENTRY("TABLE_GUID", 5, m_guid)
	PROVIDER_COLUMN_ENTRY_WSTR("DESCRIPTION", 6, m_szDesc)
	PROVIDER_COLUMN_ENTRY("TABLE_PROPID", 7, m_ulPropID)
END_PROVIDER_COLUMN_MAP()
};


class CCATALOGS_Row
{
public:
	WCHAR m_szCatalogName[129];
	WCHAR m_szDescription[129];
BEGIN_PROVIDER_COLUMN_MAP(CCATALOGS_Row)
	PROVIDER_COLUMN_ENTRY_WSTR("CATALOG_NAME", 1, m_szCatalogName)
	PROVIDER_COLUMN_ENTRY_WSTR("DESCRIPTION", 2, m_szDescription)
END_PROVIDER_COLUMN_MAP()
};

class CCOLUMNSRowEx
{
public:
	WCHAR   m_szTableCatalog[129];
	WCHAR   m_szTableSchema[129];
	WCHAR   m_szTableName[129];
	WCHAR   m_szColumnName[129];
	GUID    m_guidColumn;
	ULONG   m_ulColumnPropID;
	ULONG   m_ulOrdinalPosition;
	VARIANT_BOOL    m_bColumnHasDefault;
	WCHAR   m_szColumnDefault[129];
	ULONG   m_ulColumnFlags;
	VARIANT_BOOL    m_bIsNullable;
	USHORT  m_nDataType;
	GUID    m_guidType;
	ULONG   m_ulCharMaxLength;
	ULONG   m_ulCharOctetLength;
	USHORT  m_nNumericPrecision;
	short   m_nNumericScale;
	ULONG   m_ulDateTimePrecision;
	WCHAR   m_szCharSetCatalog[129];
	WCHAR   m_szCharSetSchema[129];
	WCHAR   m_szCharSetName[129];
	WCHAR   m_szCollationCatalog[129];
	WCHAR   m_szCollationSchema[129];
	WCHAR   m_szCollationName[129];
	WCHAR   m_szDomainCatalog[129];
	WCHAR   m_szDomainSchema[129];
	WCHAR   m_szDomainName[129];
	WCHAR   m_szDescription[129];

	CCOLUMNSRowEx()
	{
		ClearMembers();
	}

	void ClearMembers()
	{
		m_szTableCatalog[0] = NULL;
		m_szTableSchema[0] = NULL;
		m_szTableName[0] = NULL;
		m_szColumnName[0] = NULL;
		m_guidColumn = GUID_NULL;
		m_ulColumnPropID = 0;
		m_ulOrdinalPosition = 0;
		m_bColumnHasDefault = VARIANT_FALSE;
		m_szColumnDefault[0] = NULL;
		m_ulColumnFlags = 0;
		m_bIsNullable = VARIANT_FALSE;
		m_nDataType = 0;
		m_guidType = GUID_NULL;
		m_ulCharMaxLength = 0;
		m_ulCharOctetLength = 0;
		m_nNumericPrecision = 0;
		m_nNumericScale = 0;
		m_ulDateTimePrecision = 0;
		m_szCharSetCatalog[0] = NULL;
		m_szCharSetSchema[0] = NULL;
		m_szCharSetName[0] = NULL;
		m_szCollationCatalog[0] = NULL;
		m_szCollationSchema[0] = NULL;
		m_szCollationName[0] = NULL;
		m_szDomainCatalog[0] = NULL;
		m_szDomainSchema[0] = NULL;
		m_szDomainName[0] = NULL;
		m_szDescription[0] = NULL;
	}
BEGIN_PROVIDER_COLUMN_MAP(CCOLUMNSRow)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_CATALOG", 1, m_szTableCatalog)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_SCHEMA", 2, m_szTableSchema)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_NAME", 3, m_szTableName)
	PROVIDER_COLUMN_ENTRY_WSTR("COLUMN_NAME", 4, m_szColumnName)
	PROVIDER_COLUMN_ENTRY("COLUMN_GUID",5, m_guidColumn)
	PROVIDER_COLUMN_ENTRY("COLUMN_PROPID",6, m_ulColumnPropID)
	PROVIDER_COLUMN_ENTRY("ORDINAL_POSITION",7, m_ulOrdinalPosition)
	PROVIDER_COLUMN_ENTRY("COLUMN_HASDEFAULT",8, m_bColumnHasDefault)
	PROVIDER_COLUMN_ENTRY_WSTR("COLUMN_DEFAULT",9, m_szColumnDefault)
	PROVIDER_COLUMN_ENTRY("COLUMN_FLAGS",10, m_ulColumnFlags)
	PROVIDER_COLUMN_ENTRY("IS_NULLABLE",11, m_bIsNullable)
	PROVIDER_COLUMN_ENTRY("DATA_TYPE",12, m_nDataType)
	PROVIDER_COLUMN_ENTRY("TYPE_GUID",13, m_guidType)
	PROVIDER_COLUMN_ENTRY("CHARACTER_MAXIMUM_LENGTH",14, m_ulCharMaxLength)
	PROVIDER_COLUMN_ENTRY("CHARACTER_OCTET_LENGTH",15, m_ulCharOctetLength)
	PROVIDER_COLUMN_ENTRY("NUMERIC_PRECISION",16, m_nNumericPrecision)
	PROVIDER_COLUMN_ENTRY("NUMERIC_SCALE",17, m_nNumericScale)
	PROVIDER_COLUMN_ENTRY("DATETIME_PRECISION",18, m_ulDateTimePrecision)
	PROVIDER_COLUMN_ENTRY_WSTR("CHARACTER_SET_CATALOG", 19, m_szCharSetCatalog)
	PROVIDER_COLUMN_ENTRY_WSTR("CHARACTER_SET_SCHEMA", 20, m_szCharSetSchema)
	PROVIDER_COLUMN_ENTRY_WSTR("CHARACTER_SET_NAME", 21, m_szCharSetName)
	PROVIDER_COLUMN_ENTRY_WSTR("COLLATION_CATALOG", 22, m_szCollationCatalog)
	PROVIDER_COLUMN_ENTRY_WSTR("COLLATION_SCHEMA", 23, m_szCollationSchema)
	PROVIDER_COLUMN_ENTRY_WSTR("COLLATION_NAME", 24, m_szCollationName)
	PROVIDER_COLUMN_ENTRY_WSTR("DOMAIN_CATALOG", 25, m_szDomainCatalog)
	PROVIDER_COLUMN_ENTRY_WSTR("DOMAIN_SCHEMA", 26, m_szDomainSchema)
	PROVIDER_COLUMN_ENTRY_WSTR("DOMAIN_NAME", 27, m_szDomainName)
	PROVIDER_COLUMN_ENTRY_WSTR("DESCRIPTION", 28, m_szDescription)
END_PROVIDER_COLUMN_MAP()
};


class CPRIMARY_KEYRow 
{
public:
	WCHAR	m_szTableCatalog[129];
	WCHAR	m_szTableSchema[129];
	WCHAR	m_szTableName[129];
	WCHAR	m_szColumnName[129];
	GUID	m_guidColumn;
	ULONG	m_nColumnPropID;
	ULONG	m_nOrdinal;
	WCHAR	m_szPkName[129];

BEGIN_PROVIDER_COLUMN_MAP(CPRIMARY_KEYRow)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_CATALOG", 1, m_szTableCatalog)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_SCHEMA", 2, m_szTableSchema)
	PROVIDER_COLUMN_ENTRY_WSTR("TABLE_NAME", 3, m_szTableName)
	PROVIDER_COLUMN_ENTRY_WSTR("COLUMN_NAME", 4, m_szColumnName)
	PROVIDER_COLUMN_ENTRY("COLUMN_GUID", 5, m_guidColumn)
	PROVIDER_COLUMN_ENTRY("COLUMN_PROPID", 6, m_nColumnPropID)
	PROVIDER_COLUMN_ENTRY("ORDINAL", 7, m_nOrdinal)
	PROVIDER_COLUMN_ENTRY_WSTR("PK_NAME", 8, m_szPkName)
END_PROVIDER_COLUMN_MAP()
};

#define INIT_CATALOG_Type VT_BSTR
#define INIT_CATALOG_Flags (DBPROPFLAGS_DBINIT | DBPROPFLAGS_READ | DBPROPFLAGS_WRITE)
#define INIT_CATALOG_Value OLESTR("")

#endif //__ATLDBFIX_H__