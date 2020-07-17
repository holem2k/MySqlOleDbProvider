#include "stdafx.h"
#include "myobjects.h"


///////////////////////////////////////////////////////////////////////////
// CMySqlConnection

CMySqlConnection::CMySqlConnection()
{
	m_hConnection = NULL;
}

///////////////////////////////////////////////////////////////////////////

CMySqlConnection::~CMySqlConnection()
{
	InternalClose();
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlConnection::Open(LPCTSTR lpServer, LPCTSTR lpUser, 
							LPCTSTR lpPassword, LPCTSTR lpDatabase)
{
	USES_CONVERSION;

	m_hConnection = mysql_init(NULL);
	if (!m_hConnection)
		return FALSE;

	MYSQL *hConnection = mysql_real_connect(m_hConnection, T2CA(lpServer), 
		T2CA(lpUser), T2CA(lpPassword), T2CA(lpDatabase), 0, NULL,	0);
	if (!hConnection)
		m_hConnection = NULL;
	/*if success, m_hConnection == hConnection*/

	return m_hConnection != NULL;
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlConnection::Close()
{
	return InternalClose();
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlConnection::InternalClose()
{
	if (m_hConnection)
	{
		mysql_close(m_hConnection);
		m_hConnection = NULL;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CMySqlCommand

BOOL CMySqlCommand::Execute(LPCTSTR lpQuery, CMySqlConnection *pConnection, 
		RESULT_TYPE eType, LONG *pcRowsAffected, CMySqlResult **ppResult)
{
	USES_CONVERSION;

	if (!ppResult || !pConnection || (eType != RESULT_STATIC && eType != RESULT_FORWARDONLY))
		return FALSE;

	if (pcRowsAffected)
		*pcRowsAffected = 0;

	// assume error
	BOOL bResult = FALSE;
	if (pcRowsAffected)
		*pcRowsAffected = 0;
	*ppResult = NULL;

	MYSQL_RES *hResult;
	if (!mysql_real_query(pConnection->GetHandle(), T2CA(lpQuery), lstrlen(lpQuery)))
	{
		  if (eType == RESULT_STATIC)
			  hResult = mysql_store_result(pConnection->GetHandle());
		  else
			  hResult = mysql_use_result(pConnection->GetHandle());

		if (hResult)
		{
			// there are rows
			if (eType == RESULT_STATIC)
				*ppResult = new CMySqlStaticResult;
			else
				*ppResult = new CMySqlForwardResult;
			if (*ppResult)
			{
				(*ppResult)->Init(pConnection->GetHandle(), hResult);
				if (pcRowsAffected)
				{
					*pcRowsAffected = eType == RESULT_STATIC ? (UINT)mysql_num_rows(hResult)
						: -1;
				}
				bResult = TRUE;
			}
		}
		else
		{
			// no rows were returned or an error 
			if (mysql_errno(pConnection->GetHandle()) == 0)
			{
				// no rows
				if (pcRowsAffected)
					*pcRowsAffected = (UINT)mysql_affected_rows(pConnection->GetHandle());
				*ppResult = NULL;
				bResult = TRUE;
			}
		}
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CMySqlResult

CMySqlResult::CMySqlResult()
{
	m_hConnection = NULL;
	m_hResult = NULL;
	m_hRow = NULL;

	m_pColumnInfo = NULL;
	m_pStringsBuffer = NULL;
	m_nCurPos = m_cbStrBuf = m_cColumns = 0;
}

///////////////////////////////////////////////////////////////////////////

CMySqlResult::~CMySqlResult()
{
	if (m_hResult)
		mysql_free_result(m_hResult);
	if (m_pColumnInfo)
		CoTaskMemFree(m_pColumnInfo);
	if (m_pStringsBuffer)
		CoTaskMemFree(m_pStringsBuffer);
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlResult::InternalGetColumnInfo()
{
	if (!m_hResult || !m_hConnection)
		return FALSE;

	// assume error
	BOOL bResult = FALSE;
	m_cColumns = m_cbStrBuf = 0;
	m_pColumnInfo = NULL;
	m_pStringsBuffer = NULL;

	m_cColumns = mysql_num_fields(m_hResult);
	if (m_cColumns == 0)
		return FALSE; // there is no column information

	MYSQL_FIELD *pFields = mysql_fetch_fields(m_hResult); // must be OK, cause m_cColumns > 0
	m_cbStrBuf = 0;
	for (UINT i = 0; i < m_cColumns; i++)
		m_cbStrBuf += lstrlen(pFields[i].name) + 1;

	m_pColumnInfo = (COLUMNINFO *)CoTaskMemAlloc(sizeof(COLUMNINFO) * m_cColumns);
	m_pStringsBuffer = (char *)CoTaskMemAlloc(m_cbStrBuf);

	if (m_pColumnInfo && m_pStringsBuffer)
	{
		UINT uStrStart = 0;
		for (UINT i = 0; i < m_cColumns; i++)
		{
			COLUMNINFO *pColumn = m_pColumnInfo + i;
			MYSQL_FIELD *pField = pFields + i;
			BOOL bFixedLength;

			pColumn->iOrdinal = i + 1;
			m_Convertor.ConvertType(pField->type,
				pField->flags & UNSIGNED_FLAG,
				pField->length,
				&pColumn->wType,
				&pColumn->uColumnSize,
				&bFixedLength);
			if (pField->max_length > pColumn->uColumnSize)
				pColumn->uColumnSize = pField->max_length;

			// some special handling
			if (pColumn->wType == DBTYPE_DECIMAL)
			{
				pColumn->bPresion = (BYTE)pField->length;
				pColumn->bScale = (BYTE)pField->decimals;
			}
			else
				pColumn->bPresion = pColumn->bScale = ~0; // OLE DB spec.

			// set flags
			pColumn->dwFlags = 0;
			if (bFixedLength)
				pColumn->dwFlags |= CI_FIXED_LENGTH;
			if (pField->flags & PRI_KEY_FLAG)
				pColumn->dwFlags |= CI_PRIMARY_KEY;
			if (pField->flags & UNIQUE_KEY_FLAG)
				pColumn->dwFlags |= CI_UNIQUE;
			if (pField->flags & NOT_NULL_FLAG)
				pColumn->dwFlags |= CI_NOTNULL;
			if (pField->flags & AUTO_INCREMENT_FLAG)
				pColumn->dwFlags |= CI_AUTOINC;
			if (pField->flags & UNSIGNED_FLAG)
				pColumn->dwFlags |= CI_UNSIGNED;
	
			// store column name
			pColumn->lpColName = m_pStringsBuffer + uStrStart;
			lstrcpy(m_pStringsBuffer + uStrStart, pField->name);
			uStrStart += lstrlen(pField->name) + 1;
		}
		bResult = TRUE;
	}
	else
	{
		CoTaskMemFree(m_pStringsBuffer/*may be NULL*/);
		CoTaskMemFree(m_pColumnInfo/*may be NULL*/);
		m_cColumns = 0;
		m_cbStrBuf = 0;
		m_pColumnInfo = NULL;
		m_pStringsBuffer = NULL;
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlResult::GetColumnInfo(UINT *pcColumns,
								 COLUMNINFO **ppInfo,
								 char **ppStringsBuffer)
{
	if (!pcColumns || !ppInfo || !ppStringsBuffer)
	{
		if (*pcColumns)
			*pcColumns = 0;
		if (*ppInfo)
			*ppInfo = NULL;
		if (*ppStringsBuffer)
			*ppStringsBuffer = NULL;
		return FALSE;
	}

	BOOL bResult = FALSE;
	if (!m_pColumnInfo)
		InternalGetColumnInfo();

	if (m_pColumnInfo)
	{
		*ppInfo = (COLUMNINFO *)CoTaskMemAlloc(sizeof(COLUMNINFO) * m_cColumns);
		*ppStringsBuffer = (char *)CoTaskMemAlloc(m_cbStrBuf);
		if (*ppInfo && *ppStringsBuffer)
		{
			MoveMemory(*ppInfo, m_pColumnInfo, sizeof(COLUMNINFO) * m_cColumns);
			// strings special handling
			for (UINT i = 0; i < m_cColumns; i++)
			{
				(*ppInfo)[i].lpColName = (char *)((DWORD)*ppStringsBuffer +
					(DWORD)m_pColumnInfo[i].lpColName - (DWORD)m_pStringsBuffer);
			}
			MoveMemory(*ppStringsBuffer, m_pStringsBuffer, m_cbStrBuf);
			*pcColumns = m_cColumns;
			bResult = TRUE;
		}
		else
		{
			CoTaskMemFree(*ppStringsBuffer);
			CoTaskMemFree(*ppInfo);
			*ppInfo = NULL;
			*ppStringsBuffer = NULL;
			*pcColumns = 0;
		}

	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlResult::FetchRow()
{
	BOOL bResult = FALSE;
	if (m_hResult)
	{
		m_hRow = mysql_fetch_row(m_hResult);
		m_pulFieldLength = mysql_fetch_lengths(m_hResult);
		if (m_hRow && m_pulFieldLength)
		{
			m_nCurPos++;
			bResult = TRUE;
		}
	}
	return bResult;
};

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlResult::GetData(UINT iOrdinal,
						   DBTYPE wType,
						   void *pBuffer,
						   UINT cbBuf,
						   UINT *pcbData,
						   BOOL *pbNull,
						   BOOL *pbTruncated)
{
	ATLASSERT(m_hRow);
	if (pcbData == NULL || pbNull == NULL || pbTruncated == NULL)
	{
		if (pcbData != NULL)
			pcbData = 0;
		return FALSE;
	}

	*pcbData = 0;

	BOOL bResult = TRUE;
	if (!m_pColumnInfo)
		bResult = InternalGetColumnInfo();

	if (bResult && 
		iOrdinal > 0 && iOrdinal <= m_cColumns &&
		wType == m_pColumnInfo[iOrdinal - 1].wType)
	{
		bResult = FALSE;
		if (m_hRow[iOrdinal - 1])
		{
			UINT uNeededBuflen;
			switch (wType)
			{
			case DBTYPE_BYTES:
			case DBTYPE_STR:
				uNeededBuflen = 2; // empty string (may be truncated)
				break;

			default:
				uNeededBuflen = m_pColumnInfo[iOrdinal - 1].uColumnSize;
			}

			if (cbBuf >= uNeededBuflen)
			{
				*pbNull = FALSE;
				*pcbData = uNeededBuflen;
				bResult = m_Convertor.ConvertData(wType, m_hRow[iOrdinal - 1], 
					m_pulFieldLength[iOrdinal - 1], pBuffer, cbBuf, pbTruncated);
			}
		}
		else  
		{
			*pbNull = TRUE;
			*pbTruncated = FALSE;
			*pcbData = NULL;
			bResult = TRUE;
		}
	}
	else
		bResult = FALSE;
	return bResult;
}

///////////////////////////////////////////////////////////////////////////

unsigned __int64 CMySqlResult::GetCurPos()
{
	return m_nCurPos;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CMySqlStaticResult

CMySqlStaticResult::CMySqlStaticResult()
{
}

///////////////////////////////////////////////////////////////////////////

CMySqlStaticResult::~CMySqlStaticResult()
{
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlStaticResult::MoveAt(unsigned __int64 uCurPos)
{
	if (!m_hResult)
		return FALSE;

	BOOL bResult = FALSE;
	if (uCurPos < mysql_num_rows(m_hResult))
	{
		mysql_data_seek(m_hResult, uCurPos);
		m_nCurPos = uCurPos;
		bResult = TRUE;
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlStaticResult::RowCount(unsigned __int64 *puRowCount)
{
	if (!m_hResult)
		return FALSE;

	*puRowCount = mysql_num_rows(m_hResult);
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CMySqlStaticResult

CMySqlForwardResult::~CMySqlForwardResult()
{
	if (m_hResult)
	{
		// free	resources on server
		while (mysql_fetch_row(m_hResult));
	}
}

///////////////////////////////////////////////////////////////////////////

CMySqlForwardResult::MoveAt(unsigned __int64 nCurPos)
{
	// not implemented on forward only cursors
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////

BOOL CMySqlForwardResult::RowCount(unsigned __int64 *puRowCount)
{
	// not implemented on forward only cursors
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////
