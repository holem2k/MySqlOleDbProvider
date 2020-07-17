// Implementation of the CMySqlCommand
#include "stdafx.h"
#include "Prov.h"
#include "MySqlRS.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////
// CMySqlCommand

HRESULT CProvCommand::Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
								 LONG * pcRowsAffected, IUnknown ** ppRowset)
{
	HRESULT hr;
	VARIANT varForwardOnly;
	VariantInit(&varForwardOnly);
	hr = GetPropValue(&DBPROPSET_ROWSET, DBPROP_CANSCROLLBACKWARDS, &varForwardOnly);
	if (FAILED(hr))
		return hr;

	if (varForwardOnly.boolVal == VARIANT_FALSE)
	{
		CForwardRowset* pRowset;
		hr = CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRowset);
		if (SUCCEEDED(hr) && *ppRowset)
			pRowset->SetRuntimeProperties();
	}
	else
	{
		CStaticRowset *pRowset;
		hr = CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRowset);
		if (SUCCEEDED(hr) && *ppRowset)
			pRowset->SetRuntimeProperties();
	}

	if (SUCCEEDED(hr) && *ppRowset)
	{
		CComQIPtr<IIsRowset> spIsRowset(*ppRowset);
		hr = spIsRowset->IsRowset();
		if (hr == S_FALSE)
		{
			(*ppRowset)->Release(); // Destroy rowset
			*ppRowset = NULL;
			hr = S_OK;
		}
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CProvCommand::GetColumnInfo(ULONG *pcColumns,
										 DBCOLUMNINFO **prgInfo,
										 OLECHAR **ppStringsBuffer)
{
	ATLCOLUMNINFO *pInfo;
	HRESULT hr = InternalGetColumnInfo(pcColumns, &pInfo);
	if (FAILED(hr))
		return hr;

	if (*pcColumns == 0)
	{
		ATLASSERT(!pInfo);
		*prgInfo = NULL;
		*ppStringsBuffer = NULL;
		hr = S_OK;
	}
	else
		hr =  IColumnsInfoImpl<CProvCommand>::GetColumnInfo(pcColumns, prgInfo, ppStringsBuffer);
	return hr;
}

/////////////////////////////////////////////////////////////////////////////

ATLCOLUMNINFO* CProvCommand::GetColumnInfo(CProvCommand* pv, ULONG* pcInfo)
{
	ATLCOLUMNINFO *pInfo = NULL;
	if (IsCommandGenerateRows(pv->m_strCommandText) == S_OK)
	{
		HRESULT hr = pv->GetColumnInfoHelper();
		if (SUCCEEDED(hr))
		{
			*pcInfo = pv->m_cCols;
			pInfo = pv->m_pInfo;
		}
	}

	if (!pInfo)
		*pcInfo = 0;
	return pInfo;
}

/////////////////////////////////////////////////////////////////////////////

HRESULT CProvCommand::IsCommandGenerateRows(LPCOLESTR szCommandText)
{
	USES_CONVERSION;
	const wchar_t szCommand[][20] = {
		OLESTR("SELECT"),
		OLESTR("DESCRIBE"),
		OLESTR("SHOW"),
		OLESTR("EXPLAIN")
	};

	// pass spaces
	int i = 0;
	while (i < lstrlenW(szCommandText) && szCommandText[i] == ' ')
		i++;

	HRESULT hr = S_FALSE;
	for (int k = 0; k < sizeof(szCommand); k++)
	{
		if (!_wcsnicmp(OLE2CW(szCommandText + i), szCommand[i], wcslen(szCommand[i])))
		{
			hr = S_OK;
			break;
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////