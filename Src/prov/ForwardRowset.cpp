#include "stdafx.h"
#include "MySqlRS.h"
#include "error.h"

////////////////////////////////////////////////////////////////////////////
// CForwardRowset

CForwardRowset::CForwardRowset()
{
	m_pManager = NULL;
	m_pResult = NULL;
	m_pConnection = NULL;
	m_bIsRowset = FALSE;
}

////////////////////////////////////////////////////////////////////////////

HRESULT CForwardRowset::Execute(DBPARAMS * pParams, LONG* pcRowsAffected)
{
	HRESULT hr = S_OK;
	if (!InitializeRowset(pcRowsAffected))
		hr = E_FAIL;
	return hr;
}

////////////////////////////////////////////////////////////////////////////

HRESULT CForwardRowset::GetCommandFromID(DBID* pTableID, DBID* pIndexID)
{
	return GetCommandFromIDHelper(pTableID, pIndexID);
}

////////////////////////////////////////////////////////////////////////////

HRESULT CForwardRowset::ValidateCommandID(DBID* pTableID, DBID* pIndexID)
{
	HRESULT hr = _ForwardRowsetBaseClass::ValidateCommandID(pTableID, pIndexID);
	if (SUCCEEDED(hr))
		hr = ValidateCommandIDHelper(pTableID, pIndexID);
	return hr;
}

////////////////////////////////////////////////////////////////////////////

HRESULT CForwardRowset::GetDataHelper(HACCESSOR hAccessor,
		ATLCOLUMNINFO *&rpInfo,
		void **ppBinding,
		void *&rpSrcData,
		ULONG &rcCols,
		CComPtr<IDataConvert> &rspConvert,
		_HRowClass *pRow)
{
	HRESULT hr = _ForwardRowsetBaseClass::GetDataHelper(hAccessor, rpInfo, ppBinding, rpSrcData,
		rcCols, rspConvert, pRow);
	if (SUCCEEDED(hr))
	{
		IDataConvertImplProxy *pProxy = new IDataConvertImplProxy(rspConvert);
		if (pProxy == NULL)
			hr = E_OUTOFMEMORY;
		else
			rspConvert = pProxy;
	}
	return hr;
}

////////////////////////////////////////////////////////////////////////////
// IRowset::GetNextRows

STDMETHODIMP CForwardRowset::GetNextRows(HCHAPTER /*hReserved*/,
										 LONG lRowsOffset,
										 LONG cRows,
										 ULONG *pcRowsObtained,
										 HROW **prghRows)
{
	if (pcRowsObtained == NULL || prghRows == NULL)
		return E_INVALIDARG;
	if (lRowsOffset < 0)
		return DB_E_CANTSCROLLBACKWARDS;
	if (cRows < 0)
		return DB_E_CANTFETCHBACKWARDS;
	if (cRows == 0)
		return S_OK;
	
	if (!FetchNextRows(cRows + lRowsOffset))
	{
		*pcRowsObtained = 0;
		return E_FAIL;
	}
		
	
	return IRowsetImpl<CForwardRowset, IRowset, CSimpleRow>::GetNextRows(DB_NULL_HCHAPTER,
		lRowsOffset,
		cRows,
		pcRowsObtained,
		prghRows);
};

////////////////////////////////////////////////////////////////////////////
// IRowset::ReleaseRows

STDMETHODIMP CForwardRowset::ReleaseRows(ULONG cRows,
										 const HROW rghRows[],
										 DBROWOPTIONS rgRowOptions[],
										 ULONG rgRefCounts[],
										 DBROWSTATUS rgRowStatus[])
{
	if (cRows == 0)
		return S_OK;
	
	for (ULONG i = 0; i < cRows; i++)
	{
		HROW hRowCur = rghRows[i];
		_HRowClass *pRow = m_rgRowHandles.Lookup((_HRowClass::KeyType)hRowCur);
		if (pRow)
		{
			if (pRow->m_dwRef == 1) // about to die
			{
				_HRowClass::KeyType iRowset = pRow->m_iRowset;
				CDynamicStorage::RealAddress(TRUE);
				m_rgRowData.TearAt(iRowset);
				CDynamicStorage::RealAddress(FALSE);
			}
		}
	}
	return IRowsetImpl<CForwardRowset, IRowset, CSimpleRow>
		::ReleaseRows(cRows, rghRows, rgRowOptions,	rgRefCounts, rgRowStatus);
}

////////////////////////////////////////////////////////////////////////////
// IRowset::RestartPosition

STDMETHODIMP CForwardRowset::RestartPosition(HCHAPTER hReserved)
{
	UninitializeRowset();
	if (InitializeRowset(NULL))
		return IRowsetImpl<CForwardRowset, IRowset, CSimpleRow>::RestartPosition(hReserved);
	else 
		return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////

DBSTATUS CForwardRowset::GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pInfo)
{
	return m_rgRowData[pRow->m_iRowset].GetDBStatus(pInfo->iOrdinal);
}

////////////////////////////////////////////////////////////////////////////

BOOL CForwardRowset::FetchNextRows(UINT cRows)
{
	ATLASSERT(m_pResult && m_pManager);
	
	BOOL bResult = TRUE;
	do 
	{
		if (!m_pResult->FetchRow())
			break; // EOF
		
		CDynamicStorage *pStorage = m_pManager->CreateStorage(m_pResult);
		if (pStorage)
		{
			pStorage->RealAddress(TRUE);
			m_rgRowData.Add(*pStorage);
			int i = m_rgRowData.GetSize();
			m_rgRowData[i - 1].RealAddress(FALSE);
			delete pStorage;
		}
		else
		{
			bResult = FALSE;
			break;
		}
	} while (--cRows);
	
	return bResult;
}

////////////////////////////////////////////////////////////////////////////

HRESULT CForwardRowset::GetConnectionSource(IConnectionSource **pConnSource)
{
	ATLASSERT(pConnSource);
	*pConnSource = NULL;
	
	CComPtr<ICommand> spCommand;
	HRESULT hr = GetSite(IID_ICommand, (void **)&spCommand);
	if (SUCCEEDED(hr))
	{
		// if rowset creator is command
		hr = spCommand->GetDBSession(IID_IConnectionSource, (IUnknown **)pConnSource);
	}
	else
	{
		// if rowset creator is session
		hr = GetSite(IID_IConnectionSource, (void **)pConnSource);
	}
	return hr;
}

////////////////////////////////////////////////////////////////////////////

BOOL CForwardRowset::InitializeRowset(LONG *pcRowsAffected)
{
	USES_CONVERSION;
	ATLASSERT(!m_pManager);
	
	BOOL bResult = FALSE;
	
	CComPtr<IConnectionSource> spConnSource;
	HRESULT hr = GetConnectionSource(&spConnSource); 
	if (FAILED(hr))
		return FALSE;
	
	hr = spConnSource->GetConnection((void **)&m_pConnection);
	if (SUCCEEDED(hr))
	{
		CMySqlCommand command;
		BOOL bExec = command.Execute(W2T((BSTR)m_strCommandText),
			m_pConnection, 
			RESULT_FORWARDONLY,
			pcRowsAffected,
			&m_pResult);
		
		if (bExec)
		{
			if (m_pResult == NULL)
			{
				// command doesn't produce rows
				m_bIsRowset = FALSE;
				spConnSource->ReleaseConnection(m_pConnection);
				m_pConnection = NULL;
				bResult = TRUE;
			}
			else
			{
				// there are rows
				m_pManager = new CDataManager;
				if (m_pManager)
				{
					if (m_pManager->SetColumnInfo(m_pResult))
					{
						CDynamicStorage::LinkToRowset(this, m_pManager);
						m_bIsRowset = TRUE;
						bResult = TRUE;
					}
				}
			}
		}
		else
			CreateError(ERRORSRC, A2OLE(m_pConnection->GetError()), IID_IRowset);
	}
	
	if (!bResult)
	{
		if (m_pManager)
			delete m_pManager;
		if (m_pResult)
			delete m_pResult;
		if (m_pConnection)
			spConnSource->ReleaseConnection(m_pConnection);
		
		m_pResult = NULL;
		m_pConnection = NULL;
		m_pManager = NULL;
		m_bIsRowset = FALSE;
	}
	
	return bResult;
}

////////////////////////////////////////////////////////////////////////////

BOOL CForwardRowset::UninitializeRowset()
{
	if (!m_pResult)
		return TRUE;

	delete m_pResult;
	
	CComPtr<IConnectionSource> spConnSource;
	HRESULT hr = GetConnectionSource(&spConnSource); 
	if (FAILED(hr))
		return FALSE;
	spConnSource->ReleaseConnection(m_pConnection);
	
	
	// delete all rowset data 
	for (int i = 0; i < m_rgRowData.GetSize(); i++)
	{
		if (!m_rgRowData.IsTornAt(i))
			m_rgRowData[i].RealAddress(TRUE);
	}
	m_rgRowData.RemoveAll();
	CDynamicStorage::UnlinkFromRowset(this);
	delete m_pManager;

	m_pResult = NULL;
	m_pConnection = NULL;
	m_pManager = NULL;
	m_bIsRowset = FALSE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

void CForwardRowset::SetRuntimeProperties()
{
	VARIANT varVal;
	VariantInit(&varVal);
	
	varVal.vt = VT_BOOL;
	varVal.boolVal = VARIANT_FALSE;
	SetPropValue(&DBPROPSET_ROWSET, DBPROP_CANFETCHBACKWARDS, &varVal);
	SetPropValue(&DBPROPSET_ROWSET, DBPROP_CANSCROLLBACKWARDS, &varVal);
	SetPropValue(&DBPROPSET_ROWSET, DBPROP_BOOKMARKS, &varVal);
	SetPropValue(&DBPROPSET_ROWSET, DBPROP_QUICKRESTART, &varVal);

	// set linked with properties class vars
	m_bCanFetchBack = false;
	m_bCanScrollBack = false;
}

////////////////////////////////////////////////////////////////////////////

void CForwardRowset::FinalRelease()
{
	UninitializeRowset();
}

////////////////////////////////////////////////////////////////////////////