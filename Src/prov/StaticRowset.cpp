#include "stdafx.h"
#include "MySqlRS.h"
#include "error.h"

////////////////////////////////////////////////////////////////////////////
// CStaticRowset

CStaticRowset::CStaticRowset()
{
	m_pManager = NULL;
	m_pResult = NULL;
	m_pConnection = NULL;
	m_bIsRowset = FALSE;
}

////////////////////////////////////////////////////////////////////////////

HRESULT CStaticRowset::Execute(DBPARAMS * pParams, LONG* pcRowsAffected)
{
	HRESULT hr = E_FAIL;
	if (InitializeRowset(pcRowsAffected))
	{
		if (FetchAllRows())
			hr = S_OK;
	}
	return hr;
}

////////////////////////////////////////////////////////////////////////////

HRESULT CStaticRowset::GetCommandFromID(DBID* pTableID, DBID* pIndexID)
{
	return GetCommandFromIDHelper(pTableID, pIndexID);
}

////////////////////////////////////////////////////////////////////////////

HRESULT CStaticRowset::ValidateCommandID(DBID* pTableID, DBID* pIndexID)
{
	HRESULT hr = _StaticRowsetBaseClass::ValidateCommandID(pTableID, pIndexID);
	if (SUCCEEDED(hr))
		hr = ValidateCommandIDHelper(pTableID, pIndexID);
	return hr;
}

////////////////////////////////////////////////////////////////////////////

HRESULT CStaticRowset::GetDataHelper(HACCESSOR hAccessor,
		ATLCOLUMNINFO *&rpInfo,
		void **ppBinding,
		void *&rpSrcData,
		ULONG &rcCols,
		CComPtr<IDataConvert> &rspConvert,
		_HRowClass *pRow)
{
	HRESULT hr = _StaticRowsetBaseClass::GetDataHelper(hAccessor, rpInfo, ppBinding, rpSrcData,
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

HRESULT CStaticRowset::GetConnectionSource(IConnectionSource **pConnSource)
{
	ATLASSERT(pConnSource);
	*pConnSource = NULL;
	
	CComPtr<ICommand> spCommand;
	HRESULT hr = GetSite(IID_ICommand, (void **)&spCommand);
	if (SUCCEEDED(hr))
		hr = spCommand->GetDBSession(IID_IConnectionSource, (IUnknown **)pConnSource);
	else
		hr = GetSite(IID_IConnectionSource, (void **)pConnSource);
	return hr;
};

////////////////////////////////////////////////////////////////////////////

BOOL CStaticRowset::InitializeRowset(LONG *pcRowsAffected)
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
			RESULT_STATIC,
			pcRowsAffected,
			&m_pResult);

		if (bExec)
		{
			if (!m_pResult)
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
			CreateError(ERRORSRC, A2OLE(m_pConnection->GetError()), IID_IRowsetLocate);
	}

	// cleanup on failure
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

BOOL CStaticRowset::UninitializeRowset()
{
	// if there is no rowset yet
	if (!m_pResult)
		return TRUE;

	delete m_pResult;
	
	CComPtr<IConnectionSource> spConnSource;
	HRESULT hr = GetConnectionSource(&spConnSource); 
	if (FAILED(hr))
		return FALSE;
	spConnSource->ReleaseConnection(m_pConnection);
	
	CDynamicStorage::RealAddress(TRUE);
	m_rgRowData.RemoveAll();
	CDynamicStorage::RealAddress(FALSE);

	CDynamicStorage::UnlinkFromRowset(this);
	delete m_pManager;

	m_pResult = NULL;
	m_pConnection = NULL;
	m_pManager = NULL;
	m_bIsRowset = FALSE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

BOOL CStaticRowset::FetchAllRows()
{
	if (!m_bIsRowset)
		return TRUE;

	ATLASSERT(m_pResult && m_pManager);
	
	BOOL bResult = TRUE;
	while (m_pResult->FetchRow())
	{
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
	} 
	
	return bResult;
}

////////////////////////////////////////////////////////////////////////////

DBSTATUS CStaticRowset::GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pInfo)
{
	return m_rgRowData[pRow->m_iRowset].GetDBStatus(pInfo->iOrdinal);
}

////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CStaticRowset::SetSite(IUnknown* pCreator)
{
	// set member var from IRowsetLocateImpl
	CComVariant varOrderedBmks;
	HRESULT hr = GetPropValue(&DBPROPSET_ROWSET, DBPROP_ORDEREDBOOKMARKS, &varOrderedBmks);
	if (SUCCEEDED(hr))
		m_bOrderedBookmarks = varOrderedBmks.boolVal == VARIANT_TRUE;
	
	return IRowsetCreatorImpl<CStaticRowset>::SetSite(pCreator);
}

////////////////////////////////////////////////////////////////////////////

void CStaticRowset::SetRuntimeProperties()
{
	VARIANT varVal;
	VariantInit(&varVal);
	
	varVal.vt = VT_BOOL;
	varVal.boolVal = VARIANT_TRUE;
	SetPropValue(&DBPROPSET_ROWSET, DBPROP_QUICKRESTART, &varVal);

	// check bookmarks properties;
	BOOL bBookmark = FALSE;
	GetPropValue(&DBPROPSET_ROWSET, DBPROP_ORDEREDBOOKMARKS, &varVal);
	if (varVal.boolVal == VARIANT_TRUE)
		bBookmark = TRUE;
	GetPropValue(&DBPROPSET_ROWSET, DBPROP_IRowsetLocate, &varVal);
	if (varVal.boolVal == VARIANT_TRUE)
		bBookmark = TRUE;
	GetPropValue(&DBPROPSET_ROWSET, DBPROP_LITERALBOOKMARKS, &varVal);
	if (varVal.boolVal == VARIANT_TRUE)
		bBookmark = TRUE;
	if (bBookmark)
	{
		varVal.vt = VT_BOOL;
		varVal.boolVal = VARIANT_TRUE;
		SetPropValue(&DBPROPSET_ROWSET, DBPROP_BOOKMARKS, &varVal);
	}
	//GetPropValue(&DBPROPSET_ROWSET, DBPROP_BOOKMARKS, &varVal);
	//if (varVal.boolVal == VARIANT_TRUE)
	//{
	//	varVal.vt = VT_BOOL;
		//varVal.boolVal = VARIANT_TRUE;
		//SetPropValue(&DBPROPSET_ROWSET, DBPROP_IRowsetLocate, &varVal);
	//}
}

////////////////////////////////////////////////////////////////////////////

void CStaticRowset::FinalRelease()
{
	UninitializeRowset();
}

////////////////////////////////////////////////////////////////////////////