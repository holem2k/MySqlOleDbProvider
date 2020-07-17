#include "stdafx.h"
#include "MySqlDS.h"
#include "atlquerylink.h"

/////////////////////////////////////////////////////////////////////////////
// CDataSource

STDMETHODIMP CMySqlSource::GetProperties(ULONG cPropertySets,
							 const DBPROPIDSET rgPropertySets[],
							 ULONG *pcProperties,
							 DBPROPSET **prgProperties)
{
	HRESULT hr = IDBPropertiesImpl<CMySqlSource>::GetProperties(cPropertySets, rgPropertySets,
		pcProperties, prgProperties);
	if (SUCCEEDED(hr))
	{
		for (ULONG i = 0; i < *pcProperties; i++)
		{
			DBPROPSET *pSet = *prgProperties + i; 
			if (IsEqualGUID(pSet->guidPropertySet, DBPROPSET_DATASOURCE))
			{
				for (ULONG j = 0; j < pSet->cProperties; j++)
				{
					if (pSet->rgProperties[j].dwPropertyID == DBPROP_CURRENTCATALOG)
					{
						GetCurrentCatalog(&pSet->rgProperties[j].vValue);
						return hr;
					}
				}
			}
		}

	}
	return hr; 
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMySqlSource::SetProperties(ULONG cPropertySets, DBPROPSET  rgPropertySets[])
{
	HRESULT hr = IDBPropertiesImpl<CMySqlSource>::SetProperties(cPropertySets, rgPropertySets);
	
	if (SUCCEEDED(hr))
	{
		ULONG cProps = 0;
		ULONG cPropsOK = 0;
		BOOL bCatalogOK = TRUE;
		for (ULONG i = 0; i < cPropertySets; i++)
		{
			for (ULONG j = 0; j < rgPropertySets[i].cProperties; j++, cProps++)
			{
				DBPROP *pProp = rgPropertySets[i].rgProperties + j;

				if (rgPropertySets[i].guidPropertySet == DBPROPSET_DATASOURCE &&
					pProp->dwPropertyID == DBPROP_CURRENTCATALOG)
				{
					if (FAILED(SetCurrentCatalog(&pProp->vValue)))
					{
						pProp->dwStatus = DBPROPSTATUS_BADVALUE;
						bCatalogOK = FALSE;
					}
					else 
						pProp->dwStatus = DBPROPSTATUS_OK;
				}
				if (pProp->dwStatus == DBPROPSTATUS_OK)
					cPropsOK++;
			}
		}
		
		if (!bCatalogOK)
		{
			// hr == S_OK or DB_S_ERROROCCURED, change it...
			hr = cPropsOK ? DB_S_ERRORSOCCURRED : DB_E_ERRORSOCCURRED;
		}
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////

HRESULT CMySqlSource::GetCurrentCatalog(VARIANT *pvCatalog)
{
	return GetPropValue(&DBPROPSET_DATASOURCEINFO, DBPROP_CURRENTCATALOG, pvCatalog);
}

/////////////////////////////////////////////////////////////////////////////

HRESULT CMySqlSource::SetCurrentCatalog(VARIANT *pvCatalog)
{
	USES_CONVERSION;

	ATLASSERT(pvCatalog);
	if (pvCatalog->vt != VT_BSTR)
		return E_INVALIDARG;
	if (wcslen(pvCatalog->bstrVal) > 64)
		return E_INVALIDARG;

	CMySqlConnection *pConnection;
	HRESULT hr = GetConnection((void **)&pConnection);
	if (SUCCEEDED(hr))
	{
		TCHAR lpQuery[4 + 64 + 1];
		lstrcpy(lpQuery, _T("USE "));
		lstrcat(lpQuery, OLE2T(pvCatalog->bstrVal));
		CSimpleQuery query(IID_IDBProperties);

		if (query.Execute(pConnection, lpQuery, pvCatalog))
		{
			// catalog name is valid
			SetPropValue(&DBPROPSET_DATASOURCEINFO, DBPROP_CURRENTCATALOG, pvCatalog);
		}
		else
			hr = E_FAIL;

		ReleaseConnection(pConnection);
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMySqlSource::GetConnection(void **ppConnection)
{
	USES_CONVERSION;
	if (!ppConnection)
		return E_INVALIDARG;
	
	*ppConnection = NULL;
	
	CDBPropIDSet set[2];
	set[0].SetGUID(DBPROPSET_DBINIT);
	set[1].SetGUID(DBPROPSET_DATASOURCE);
	set[0].AddPropertyID(DBPROP_INIT_DATASOURCE); // server
	set[0].AddPropertyID(DBPROP_AUTH_USERID);     // user
	set[0].AddPropertyID(DBPROP_AUTH_PASSWORD);   // password
	set[1].AddPropertyID(DBPROP_CURRENTCATALOG);   // database
	
	ULONG cPropertySets;
	DBPROPSET *rgPropSets;
	
	HRESULT hr = GetProperties(2, set, &cPropertySets, &rgPropSets);
	if (rgPropSets)
	{
		if (SUCCEEDED(hr))
		{
			CMySqlConnection *pConnection;
			pConnection = new CMySqlConnection;
			if (pConnection)
			{
				LPTSTR lpServer = W2T(rgPropSets[0].rgProperties[0].vValue.bstrVal);
				LPTSTR lpUser  = W2T(rgPropSets[0].rgProperties[1].vValue.bstrVal);
				LPTSTR lpPassword  = W2T(rgPropSets[0].rgProperties[2].vValue.bstrVal);
				LPTSTR lpDatabase  = W2T(rgPropSets[1].rgProperties[0].vValue.bstrVal);
				
				if (pConnection->Open(lpServer, lpUser,	lpPassword, lpDatabase))
					*ppConnection = pConnection;
				else
				{
					hr = E_FAIL;
					CreateError(ERRORSRC, OLESTR("Connection to server failed."), IID_NULL);
				}
			}
		}
		// free memory 
		CoTaskMemFree(rgPropSets[0].rgProperties);
		CoTaskMemFree(rgPropSets[1].rgProperties);
		CoTaskMemFree(rgPropSets);
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CMySqlSource::ReleaseConnection(void *pConnection)
{
	delete (CMySqlConnection *)pConnection;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

HRESULT CMySqlSource::OnPropertyChanged(ULONG iCurSet, DBPROP *pDBProp)
{
	if (pDBProp->dwPropertyID == DBPROP_INIT_CATALOG)
	{
		CComVariant vCatalog;
		GetPropValue(&DBPROPSET_DBINIT, DBPROP_INIT_CATALOG, &vCatalog);
		SetPropValue(&DBPROPSET_DATASOURCE, DBPROP_CURRENTCATALOG, &vCatalog); 
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////