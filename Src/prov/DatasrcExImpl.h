#include "stdafx.h"

const LPOLESTR szKeywords[] = 
{
	OLESTR("AGGREGATE"),
	OLESTR("AFTER"),
	OLESTR("AVG_ROW_LENGTH"),
	OLESTR("AUTO_INCREMENT"),
	OLESTR("BIGINT"),
	OLESTR("BINARY"),
	OLESTR("BLOB"),
	OLESTR("BOOL"),
	OLESTR("CHANGE"),
	OLESTR("CHECKSUM"),
	OLESTR("COLUMNS"),
	OLESTR("COMMENT"),
	OLESTR("DATA"),
	OLESTR("DATABASE"),
	OLESTR("DATABASES"),
	OLESTR("DATETIME"),
	OLESTR("DAY_HOUR"),
	OLESTR("DAY_MINUTE"),
	OLESTR("DAY_SECOND"),
	OLESTR("DAYOFMONTH"),
	OLESTR("DAYOFWEEK"),
	OLESTR("DAYOFYEAR"),
	OLESTR("DELAYED"),
	OLESTR("DELAY_KEY_WRITE"),
	OLESTR("ESCAPED"),
	OLESTR("ENCLOSED"),
	OLESTR("ENUM"),
	OLESTR("EXPLAIN"),
	OLESTR("FIELDS"),
	OLESTR("FILE"),
	OLESTR("FLOAT4"),
	OLESTR("FLOAT8"),
	OLESTR("FLUSH"),
	OLESTR("FUNCTION"),
	OLESTR("GRANTS"),
	OLESTR("HEAP"),
	OLESTR("HIGH_PRIORITY"),
	OLESTR("HOUR_MINUTE"),
	OLESTR("HOUR_SECOND"),
	OLESTR("HOSTS"),
	OLESTR("IDENTIFIED"),
	OLESTR("IGNORE"),
	OLESTR("INDEX"),
	OLESTR("INFILE"),
	OLESTR("INSERT_ID"),
	OLESTR("INT1"),
	OLESTR("INT2"),
	OLESTR("INT3"),
	OLESTR("INT4"),
	OLESTR("INT8"),
	OLESTR("IF"),
	OLESTR("ISAM"),
	OLESTR("KEYS"),
	OLESTR("KILL"),
	OLESTR("LAST_INSERT_ID"),
	OLESTR("LENGTH"),
	OLESTR("LINES"),
	OLESTR("LIMIT"),
	OLESTR("LOAD"),
	OLESTR("LOCK"),
	OLESTR("LOGS"),
	OLESTR("LONG"),
	OLESTR("LONGBLOB"),
	OLESTR("LONGTEXT"),
	OLESTR("LOW_PRIORITY"),
	OLESTR("MAX_ROWS"),
	OLESTR("MEDIUMBLOB"),
	OLESTR("MEDIUMTEXT"),
	OLESTR("MEDIUMINT"),
	OLESTR("MIDDLEINT"),
	OLESTR("MIN_ROWS"),
	OLESTR("MINUTE_SECOND"),
	OLESTR("MODIFY"),    
	OLESTR("MONTHNAME"), 
	OLESTR("MYISAM"),         
	OLESTR("OPTIMIZE"),
	OLESTR("OPTIONALLY"),
	OLESTR("OUTFILE"),
	OLESTR("PACK_KEYS"),
	OLESTR("PASSWORD"),
	OLESTR("PROCESS"),
	OLESTR("PROCESSLIST"),
	OLESTR("RELOAD"),
	OLESTR("REGEXP"),
	OLESTR("RENAME"),
	OLESTR("REPLACE"),
	OLESTR("RETURNS"),
	OLESTR("RLIKE"),          
	OLESTR("ROW"),
	OLESTR("SHOW"),
	OLESTR("SHUTDOWN"),
	OLESTR("SONAME"),
	OLESTR("SQL_BIG_TABLES"),
	OLESTR("SQL_BIG_SELECTS"),
	OLESTR("SQL_LOW_PRIORITY_UPDATES"),
	OLESTR("SQL_LOG_OFF"),
	OLESTR("SQL_LOG_UPDATE"),
	OLESTR("SQL_SELECT_LIMIT"),
	OLESTR("SQL_SMALL_RESULT"),
	OLESTR("SQL_BIG_RESULT"),  
	OLESTR("SQL_WARNINGS"),
	OLESTR("STRAIGHT_JOIN"),
	OLESTR("STARTING"),
	OLESTR("STATUS"),
	OLESTR("STRING"),
	OLESTR("TABLES"),
	OLESTR("TERMINATED"),
	OLESTR("TEXT"),
	OLESTR("TINYBLOB"),
	OLESTR("TINYTEXT"),
	OLESTR("TINYINT"),
	OLESTR("TYPE"),
	OLESTR("USE"),
	OLESTR("UNLOCK"),
	OLESTR("UNSIGNED"),
	OLESTR("VARIABLES"),
	OLESTR("VARBINARY"),
	OLESTR("YEAR_MONTH"),
	OLESTR("ZEROFILL")
};

const DBLITERALINFO rgLiteralInfo[] =
{
	{NULL, OLESTR("!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), OLESTR("0123456789!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), DBLITERAL_CATALOG_NAME, TRUE, 64},
	{NULL, OLESTR("!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), OLESTR("0123456789!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), DBLITERAL_VIEW_NAME, TRUE, 64},
	{NULL, OLESTR("!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), OLESTR("0123456789!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), DBLITERAL_TABLE_NAME, TRUE, 64},
	{NULL, OLESTR("!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), OLESTR("0123456789!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), DBLITERAL_CORRELATION_NAME, TRUE, 64},
	{NULL, OLESTR("!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), OLESTR("0123456789!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), DBLITERAL_COLUMN_NAME, TRUE, 64},
	{NULL, OLESTR("!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), OLESTR("0123456789!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), DBLITERAL_COLUMN_ALIAS, TRUE, 64},
	{NULL, OLESTR("!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), OLESTR("0123456789!\"%&'()*_+,-./:;<=>?@[\\]^{|}~"), DBLITERAL_INDEX_NAME, TRUE, 64},
	{NULL, NULL, NULL, DBLITERAL_TEXT_COMMAND, TRUE, 8192},
	{NULL, NULL, NULL, DBLITERAL_USER_NAME, TRUE, 16},
	{OLESTR("."), NULL, NULL, DBLITERAL_CATALOG_SEPARATOR, TRUE, 1},
	{OLESTR("%"), NULL, NULL, DBLITERAL_LIKE_PERCENT, TRUE, 1},
	{OLESTR("_"), NULL, NULL, DBLITERAL_LIKE_UNDERSCORE, TRUE, 1},
	{OLESTR("\\")/*\*/, NULL, NULL, DBLITERAL_ESCAPE_PERCENT_PREFIX, TRUE, 1},
	{OLESTR("\\")/*\*/, NULL, NULL, DBLITERAL_ESCAPE_UNDERSCORE_PREFIX, TRUE, 1},
	//{OLESTR("'"), NULL, NULL, DBLITERAL_QUOTE_PREFIX, TRUE, 1},
	//{OLESTR("'"), NULL, NULL, DBLITERAL_QUOTE_SUFFIX, TRUE, 1},
	//{OLESTR("{}"), NULL, NULL, DBLITERAL_QUOTE, TRUE, 2}
};

/////////////////////////////////////////////////////////////////////////
// IDBInfoImpl

template <class T>
class IDBInfoImpl : public IDBInfo
{
	STDMETHOD(GetKeywords)(
		LPOLESTR *ppwszKeywords)
	{
		ATLTRACE2(atlTraceDBProvider, 0, "IDBInfoImpl::GetKeywords\n");
		T *pT = (T *)this;
		if (!(pT->m_dwStatus & DSF_INITIALIZED))
			return E_UNEXPECTED;
		if (!ppwszKeywords)
			return E_INVALIDARG;

		UINT cKeywords = sizeof(szKeywords)/sizeof(szKeywords[0]);
		UINT cbKeywords = 0;
		for (UINT i = 0; i < cKeywords; i++)
			cbKeywords += lstrlenW(szKeywords[i]) + 1;
		cbKeywords = ++cbKeywords*sizeof(OLECHAR);
		
		HRESULT hr;
		LPOLESTR szKeywordsBuf = (LPOLESTR)CoTaskMemAlloc(cbKeywords);
		if (szKeywords)
		{
			UINT cbOffset = 0;
			for (UINT i = 0; i < cKeywords; i++)
			{
				lstrcpyW(szKeywordsBuf + cbOffset, szKeywords[i]);
				cbOffset += lstrlenW(szKeywords[i]);
				*(szKeywordsBuf + cbOffset++) = OLESTR(',');
			}
			*(szKeywordsBuf + cbOffset - 1) = OLESTR('\0');
			*ppwszKeywords = szKeywordsBuf;
			hr = S_OK;
		}
		else
		{
			*ppwszKeywords = NULL;
			hr = E_OUTOFMEMORY;
		}
		return hr;
	}
	STDMETHOD(GetLiteralInfo)(
		ULONG              cLiterals,
		const DBLITERAL    rgLiterals[],
		ULONG             *pcLiteralInfo,
		DBLITERALINFO    **prgLiteralInfo,
		OLECHAR          **ppCharBuffer)
	{
		ATLTRACE2(atlTraceDBProvider, 0, "IDBInfoImpl::GetLiteralInfo\n");
		T *pT = (T *)this;
		if (!(pT->m_dwStatus & DSF_INITIALIZED))
			return E_UNEXPECTED;
		if (cLiterals > 0 && rgLiterals == NULL)
			return E_INVALIDARG;
		if (prgLiteralInfo == NULL || ppCharBuffer == NULL || pcLiteralInfo == NULL)
			return E_INVALIDARG;

		// assume error
		*pcLiteralInfo = 0;
		*prgLiteralInfo = NULL;
		*ppCharBuffer = NULL;

		ULONG cRealLiterals = sizeof(rgLiteralInfo)/sizeof(rgLiteralInfo[0]);

		HRESULT hr;
		if (cLiterals == 0)
		{
			ULONG cbCharBuffer = 0;
			for (UINT i = 0; i < cRealLiterals; i++)
				cbCharBuffer += LiteralInfoStringsLength(rgLiteralInfo + i);

			DBLITERALINFO *rgInfo = (DBLITERALINFO *)CoTaskMemAlloc(cRealLiterals * sizeof(DBLITERALINFO));
			OLECHAR *pCharBuffer = (OLECHAR *)CoTaskMemAlloc(cbCharBuffer);
			if (rgInfo && pCharBuffer)
			{
				ULONG nOffset = 0;
				for (UINT i = 0; i < cRealLiterals; i++)
					CopyLiteralInfo(rgInfo + i, rgLiteralInfo + i, pCharBuffer, nOffset);

				*pcLiteralInfo = cRealLiterals;
				*prgLiteralInfo = rgInfo;
				*ppCharBuffer = pCharBuffer;

				hr = S_OK;
			}
			else
			{
				CoTaskMemFree(rgInfo);
				CoTaskMemFree(pCharBuffer);
				hr = E_OUTOFMEMORY;
			}
		}
		else
		{
			INT *pIndex;
			ATLTRY(pIndex = new INT[cLiterals]);
			if (pIndex == NULL)
				return E_OUTOFMEMORY;

			ULONG cErrors = 0;
			ULONG cbCharBuffer = 0;
			for (ULONG i = 0; i < cLiterals; i++)
			{
				pIndex[i] = -1;
				for (ULONG j = 0; j < cRealLiterals; j++)
				{
					
					if (rgLiteralInfo[j].lt == rgLiterals[i])
					{
						pIndex[i] = j;
						cbCharBuffer += LiteralInfoStringsLength(rgLiteralInfo + j);
						break;
					}
				}
				if (pIndex[i] == -1)
					cErrors++;
			}

			DBLITERALINFO *rgInfo = (DBLITERALINFO *)CoTaskMemAlloc(cLiterals * sizeof(DBLITERALINFO));
			OLECHAR *pCharBuffer = (OLECHAR *)CoTaskMemAlloc(cbCharBuffer);
			if (rgInfo && pCharBuffer)
			{
				ULONG cbOffset = 0;
				for (UINT i = 0; i < cLiterals; i++)
				{
					if (pIndex[i] != -1)
						CopyLiteralInfo(rgInfo + i, rgLiteralInfo + pIndex[i], pCharBuffer, cbOffset);
					else
						CopyUnsupportedLiteralInfo(rgInfo + i, rgLiterals[i]);
				}

				*pcLiteralInfo = cLiterals;
				*prgLiteralInfo = rgInfo;
				*ppCharBuffer = pCharBuffer;

				if (cErrors == cLiterals)
					hr = DB_E_ERRORSOCCURRED;
				else if (cErrors > 0)
					hr = DB_S_ERRORSOCCURRED;
				else
					hr = S_OK;
			}
			else
			{
				CoTaskMemFree(rgInfo);
				CoTaskMemFree(pCharBuffer);
				hr = E_OUTOFMEMORY;
			}
			delete [] pIndex;
		}
		return hr;
	}

protected:
	void CopyLiteralInfo(DBLITERALINFO *pDestInfo, const DBLITERALINFO *pSrcInfo, OLECHAR *pBuffer, ULONG &nOffset)
	{
		pDestInfo->lt = pSrcInfo->lt;
		ATLASSERT(pSrcInfo->fSupported);
		pDestInfo->fSupported = pSrcInfo->fSupported;
		pDestInfo->cchMaxLen = pSrcInfo->cchMaxLen;
		
		if (pSrcInfo->pwszLiteralValue)
		{
			pDestInfo->pwszLiteralValue = pBuffer + nOffset;
			lstrcpyW(pDestInfo->pwszLiteralValue, pSrcInfo->pwszLiteralValue);
			nOffset += lstrlenW(pDestInfo->pwszLiteralValue) + 1;
		}
		else
			pDestInfo->pwszLiteralValue = NULL;
		
		if (pSrcInfo->pwszInvalidStartingChars)
		{
			pDestInfo->pwszInvalidStartingChars = pBuffer + nOffset;
			lstrcpyW(pDestInfo->pwszInvalidStartingChars, pSrcInfo->pwszInvalidStartingChars);
			nOffset += lstrlenW(pDestInfo->pwszInvalidStartingChars) + 1;
		}
		else
			pDestInfo->pwszInvalidStartingChars = NULL;
		
		if (pSrcInfo->pwszInvalidChars)
		{
			pDestInfo->pwszInvalidChars = pBuffer + nOffset;
			lstrcpyW(pDestInfo->pwszInvalidChars, pSrcInfo->pwszInvalidChars);
			nOffset += lstrlenW(pDestInfo->pwszInvalidChars) + 1;
		}
		else
			pDestInfo->pwszInvalidChars = NULL;
		
	}
	void CopyUnsupportedLiteralInfo(DBLITERALINFO *pDestInfo, const DBLITERAL lt)
	{
		pDestInfo->pwszLiteralValue = NULL;
		pDestInfo->pwszInvalidStartingChars = NULL;
		pDestInfo->pwszInvalidChars = NULL;
		pDestInfo->lt = lt;
		pDestInfo->fSupported = FALSE;
		pDestInfo->cchMaxLen = ~0;
	}
	ULONG LiteralInfoStringsLength(const DBLITERALINFO *pInfo)
	{
		UINT cLen = 0;
		if (pInfo->pwszLiteralValue)
			cLen += lstrlenW(pInfo->pwszLiteralValue) + 1;
		if (pInfo->pwszInvalidStartingChars)
			cLen += lstrlenW(pInfo->pwszInvalidStartingChars) + 1;
		if (pInfo->pwszInvalidChars)
			cLen += lstrlenW(pInfo->pwszInvalidChars) + 1;
		return cLen*2;
	}
};
