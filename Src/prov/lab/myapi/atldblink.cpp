#include "stdafx.h"
#include "atldblink.h"
//#include "crtdbg.h"

const UINT INVALID_STORAGE = 0;

///////////////////////////////////////////////////////////////////////////
// CDynamicStorage

CSimpleMap<void *, CDataManager *> CDynamicStorage::m_rgRowsetMap;
CComAutoCriticalSection CDynamicStorage::m_section;
BOOL CDynamicStorage::m_bRealAddress = TRUE;

CDynamicStorage::CDynamicStorage()
{
	m_pManager = NULL;
	m_nStorage = INVALID_STORAGE;
	m_bRealAddress = TRUE;
}

///////////////////////////////////////////////////////////////////////////

CDynamicStorage::CDynamicStorage(CDataManager *pManager, UINT nStorage)
{
	m_pManager = pManager;
	m_nStorage = nStorage;
	//m_bRealAddress = FALSE;
	m_bRealAddress = TRUE;
	m_pManager->AddStorageDataRef(m_nStorage);
}

///////////////////////////////////////////////////////////////////////////

CDynamicStorage::CDynamicStorage(const CDynamicStorage &storage)
{
	m_nStorage = storage.m_nStorage;
	m_pManager = storage.m_pManager;
	m_bRealAddress = storage.m_bRealAddress;
	ATLASSERT(m_pManager);
	m_pManager->AddStorageDataRef(m_nStorage);
}

///////////////////////////////////////////////////////////////////////////

CDynamicStorage::~CDynamicStorage()
{
	ATLASSERT(m_pManager);
	m_pManager->ReleaseStorageData(m_nStorage);
}

///////////////////////////////////////////////////////////////////////////
// used by ATL OLE DB templates

ATLCOLUMNINFO *CDynamicStorage::GetColumnInfo(void *pThis, ULONG *pcCols)
{
	// its static function of class, so many apartments can
	// call it simultaneously, lock this rubbish.
	m_section.Lock();

	CComQIPtr<IRowsetInfo, &IID_IRowsetInfo> spRI((IUnknown *)pThis);

	CDBPropIDSet set(DBPROPSET_ROWSET);
	set.AddPropertyID(DBPROP_BOOKMARKS);
	ULONG cPropertySets = 0;
	DBPROPSET *pPropSet = NULL;
	BOOL bBmk = FALSE;

	HRESULT hr = spRI->GetProperties(1, &set, &cPropertySets, &pPropSet);
	if (&pPropSet)
	{
		if (SUCCEEDED(hr) && pPropSet[0].rgProperties[0].vValue.boolVal == VARIANT_TRUE)
			bBmk = TRUE;

		CoTaskMemFree(pPropSet[0].rgProperties);
		CoTaskMemFree(pPropSet);
	}

	CDataManager *pManager = m_rgRowsetMap.Lookup(pThis);
	m_section.Unlock();
	ATLASSERT(pManager);
	return pManager->GetColumnInfo(bBmk, pcCols);
}

///////////////////////////////////////////////////////////////////////////

BOOL CDynamicStorage::LinkToRowset(void *pRowset, CDataManager *pManager)
{
	m_section.Lock();
	BOOL bResult = m_rgRowsetMap.Add(pRowset, pManager);
	m_section.Unlock();
	return bResult;
}

///////////////////////////////////////////////////////////////////////////

BOOL CDynamicStorage::UnlinkFromRowset(void *pRowset)
{
	m_section.Lock();
	BOOL bResult = m_rgRowsetMap.Remove(pRowset);
	m_section.Unlock();
	return bResult;
}

///////////////////////////////////////////////////////////////////////////
// used by ATL OLE DB templates


CDynamicStorage *CDynamicStorage::operator &()
{
	ATLASSERT(m_pManager);
	if (m_bRealAddress)
		return this;
	else
		return (CDynamicStorage *)m_pManager->GetStorageData(m_nStorage);
}

///////////////////////////////////////////////////////////////////////////

CDynamicStorage &CDynamicStorage::operator =(const CDynamicStorage &storage)
{
	if (this != &storage)
	{
		m_nStorage = storage.m_nStorage;
		m_bRealAddress = storage.m_bRealAddress;
		m_pManager = storage.m_pManager;
		ATLASSERT(m_pManager);
		m_pManager->AddStorageDataRef(m_nStorage);
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////

DBSTATUS CDynamicStorage::GetDBStatus(ULONG iOrdinal)
{
	ATLASSERT(m_pManager);
	return m_pManager->GetStorageStatus(m_nStorage, iOrdinal);
}

///////////////////////////////////////////////////////////////////////////

void CDynamicStorage::RealAddress(BOOL bRealAddress)
{
	m_bRealAddress = bRealAddress;
}

///////////////////////////////////////////////////////////////////////////
// CDataManager

CDataManager::CDataManager()
{
	m_uNextStorageNum = 1;
	m_pInfo = NULL;
	m_pStringsBuffer = NULL;
	m_cbData = 0;
	m_cCols = 0;
}

///////////////////////////////////////////////////////////////////////////

CDataManager::~CDataManager()
{
	ATLASSERT(m_rgStorageData.GetSize() == 0);
	delete [] m_pInfo;
	delete [] m_pStringsBuffer;
}

///////////////////////////////////////////////////////////////////////////

CDynamicStorage *CDataManager::CreateStorage(CMySqlResult *pResult)
{
	ATLASSERT(pResult);

	if (!m_pInfo)
		return NULL;

	CDynamicStorage *pStorage = NULL;
	BYTE *pData = new BYTE[m_cbData];
	DBSTATUS *pStatus = new DBSTATUS[m_cCols + 1/*bookmark*/];
	DATADESC *pDesc = new DATADESC;
	if (pData && pDesc && pStatus)
	{
		if (GetDataFromResult(pResult, pData, pStatus))
		{
			pDesc->cRef = 0;
			pDesc->pData = pData;
			pDesc->pStatus = pStatus;
			if (m_rgStorageData.Add(m_uNextStorageNum, pDesc))
				pStorage = new CDynamicStorage(this, m_uNextStorageNum);
		}
	}

	if (!pStorage)
	{
		delete [] pData; /* may be NULL */
		delete [] pStatus; /* may be NULL */
		delete pDesc; /* may be NULL */
	}
	else
		m_uNextStorageNum++;

	return pStorage;
}

///////////////////////////////////////////////////////////////////////////

BOOL CDataManager::SetColumnInfo(CMySqlResult *pResult)
{
	ATLASSERT(pResult);

	USES_CONVERSION;

	UINT cCols;
	COLUMNINFO *pInfo;
	char *pStrBuf;

	BOOL bResult = FALSE;
	if (pResult->GetColumnInfo(&cCols, &pInfo, &pStrBuf))
	{
		UINT cbStrBuf = 0;
		for (UINT i = 0; i < cCols; i++)
			cbStrBuf += (lstrlen(pInfo[i].lpColName) + 1);
		cbStrBuf *= sizeof(OLECHAR);

		// Allocate space for all columns and for bookmark column
		m_pInfo = new ATLCOLUMNINFO[cCols + 1];
		m_pStringsBuffer = new OLECHAR[cbStrBuf];
		m_cCols = cCols;
		UINT uNextStr = 0;
		if (m_pInfo && m_pStringsBuffer)
		{
			// set bookmark column
			ATLCOLUMNINFO *pAtlColumn = m_pInfo;
			pAtlColumn->iOrdinal = 0;
			pAtlColumn->wType = DBTYPE_I4;
			pAtlColumn->bPrecision = GetTypePrecision(DBTYPE_I4);
			pAtlColumn->bScale = ~0;
			pAtlColumn->ulColumnSize = sizeof(int);
			pAtlColumn->dwFlags = DBCOLUMNFLAGS_ISBOOKMARK | 
				DBCOLUMNFLAGS_ISFIXEDLENGTH;
			pAtlColumn->pTypeInfo = NULL;
			pAtlColumn->pwszName = NULL;
			pAtlColumn->columnid.eKind = DBKIND_GUID_PROPID;
			pAtlColumn->columnid.uGuid.guid = DBCOL_SPECIALCOL;
			pAtlColumn->columnid.uName.ulPropid = 2;
			pAtlColumn->cbOffset = 0;			

			for (i = 0; i < cCols; i++)
			{
				ATLCOLUMNINFO *pAtlColumn = m_pInfo + i + 1;
				COLUMNINFO *pColumn = pInfo + i;

				//// fill ATL column information
				pAtlColumn->iOrdinal = pColumn->iOrdinal;
				pAtlColumn->wType = pColumn->wType;
				pAtlColumn->bPrecision = GetTypePrecision(pColumn->wType);
				pAtlColumn->bScale = pColumn->bScale;
				pAtlColumn->pTypeInfo = NULL;
				pAtlColumn->ulColumnSize = pColumn->uColumnSize;

				// set flags
				DWORD dwFlags = 0;
				if (pColumn->dwFlags & CI_FIXED_LENGTH)
					dwFlags |= DBCOLUMNFLAGS_ISFIXEDLENGTH;
				if (!(pColumn->dwFlags & CI_NOTNULL))
					dwFlags |= DBCOLUMNFLAGS_ISNULLABLE | DBCOLUMNFLAGS_MAYBENULL;
				pAtlColumn->dwFlags = dwFlags;

				// set cbOffset
				pAtlColumn->cbOffset = m_pInfo[i].cbOffset + m_pInfo[i].ulColumnSize;
				// if string, reserve space for terminating zero 
				// if BLOB, reserve space for size
				if (m_pInfo[i].wType == DBTYPE_STR)
					pAtlColumn->cbOffset += sizeof(char);
				else if (m_pInfo[i].wType == DBTYPE_WSTR)
					pAtlColumn->cbOffset += sizeof(wchar_t);
				else if (m_pInfo[i].wType == DBTYPE_BYTES)
					pAtlColumn->cbOffset += sizeof(ULONG);

				// set column name and id
				lstrcpyW(m_pStringsBuffer + uNextStr, A2W(pColumn->lpColName));
				pAtlColumn->pwszName = m_pStringsBuffer + uNextStr;
				pAtlColumn->columnid.eKind = DBKIND_NAME;
				pAtlColumn->columnid.uName.pwszName = m_pStringsBuffer + uNextStr;
				uNextStr += lstrlenW(m_pStringsBuffer + uNextStr) + 1;
			}

			m_cbData = m_pInfo[cCols].cbOffset + m_pInfo[cCols].ulColumnSize;
			// if string, reserve space for terminating zero 
			// if BLOB, reserve space for size
			if (m_pInfo[cCols].wType == DBTYPE_STR)
				m_cbData += sizeof(char);
			else if (m_pInfo[cCols].wType == DBTYPE_WSTR)
				m_cbData += sizeof(wchar_t);
			else if (m_pInfo[cCols].wType == DBTYPE_BYTES)
				m_cbData += sizeof(ULONG);
			bResult = TRUE;
		}
		else
		{
			CoTaskMemFree(m_pInfo/*may be NULL*/);
			CoTaskMemFree(m_pStringsBuffer/*may be NULL*/);
			m_pInfo = NULL;
			m_pStringsBuffer = NULL;
		}

		CoTaskMemFree(pInfo);
		CoTaskMemFree(pStrBuf);
	}
	return bResult;
}


///////////////////////////////////////////////////////////////////////////

void *CDataManager::GetStorageData(UINT nStorage)
{
	DATADESC *pDesc = m_rgStorageData.Lookup(nStorage);
	return pDesc ? (void *)pDesc->pData : NULL;
}


///////////////////////////////////////////////////////////////////////////

void CDataManager::AddStorageDataRef(UINT nStorage)
{
	DATADESC *pDesc = m_rgStorageData.Lookup(nStorage);
	if (pDesc)
		pDesc->cRef++;
}

///////////////////////////////////////////////////////////////////////////

void CDataManager::ReleaseStorageData(UINT nStorage)
{
	DATADESC *pDesc = m_rgStorageData.Lookup(nStorage);
	if (pDesc)
	{
		if(!--pDesc->cRef)
		{
			//_CrtMemDumpAllObjectsSince();
			delete [] pDesc->pData;
			delete [] pDesc->pStatus;
			delete pDesc;
			m_rgStorageData.Remove(nStorage);
		}
	}
}

///////////////////////////////////////////////////////////////////////////

ATLCOLUMNINFO *CDataManager::GetColumnInfo(BOOL bBookmarks, ULONG *pcCols)
{
	if (!m_pInfo)
	{
		*pcCols = 0;
		return NULL;
	}

	if (bBookmarks)
		*pcCols = m_cCols + 1;
	else
		*pcCols = m_cCols;
	return m_pInfo + m_cCols - *pcCols + 1;
}

///////////////////////////////////////////////////////////////////////////

DBSTATUS CDataManager::GetStorageStatus(UINT nStorage, ULONG iOrdinal)
{
	ATLASSERT(iOrdinal <= m_cCols);

	DATADESC *pDesc = m_rgStorageData.Lookup(nStorage);
	return pDesc ? pDesc->pStatus[iOrdinal] : DBSTATUS_E_UNAVAILABLE;
}

///////////////////////////////////////////////////////////////////////////

BYTE CDataManager::GetTypePrecision(DBTYPE wType)
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

///////////////////////////////////////////////////////////////////////////

BOOL CDataManager::GetDataFromResult(CMySqlResult *pResult, BYTE *pData, DBSTATUS *pStatus)
{
	if (!m_pInfo)
		return FALSE;

	BOOL bResult = TRUE;

	// set bookmark column
	*(UINT *)pData = (UINT)pResult->GetCurPos() + 256;
	pStatus[0] = DBSTATUS_S_OK;

	// set ordinary columns
	for (UINT i = 0; i < m_cCols; i++)
	{
		ATLCOLUMNINFO *pColumn = m_pInfo + i + 1;
		BOOL bNull;
		BOOL bTruncated;
		UINT cbData;
		UINT cbBuf = pColumn->ulColumnSize;
		if (pColumn->wType == DBTYPE_STR)
			cbBuf += sizeof(char);
		else if (pColumn->wType == DBTYPE_WSTR)
			cbBuf += sizeof(wchar_t);

		if (!pResult->GetData(i + 1, 
			pColumn->wType,
			pData + pColumn->cbOffset,
			cbBuf,
			&cbData,
			&bNull,
			&bTruncated))
		{
			bResult = FALSE;
			break;
		}

		if (bNull)
			pStatus[i + 1] = DBSTATUS_S_ISNULL;
		else if (bTruncated)
			pStatus[i + 1] = DBSTATUS_S_TRUNCATED;
		else 
			pStatus[i + 1] = DBSTATUS_S_OK;
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////
