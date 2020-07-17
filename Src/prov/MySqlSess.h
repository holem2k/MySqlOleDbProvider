// Session.h : Declaration of the CMySqlSession
#ifndef __CMySqlSession_H_
#define __CMySqlSession_H_
#include "resource.h"       // main symbols
#include "atldbfix.h"
#include "MySqlRS.h"
#include "prov.h"
#include "lab\myapi\myobjects.h"
#include "error.h"


class CMySqlSessionCatalogsRowset;
class CMySqlSessionTRSchemaRowset;
class CMySqlSessionColSchemaRowset;
class CMySqlSessionPTSchemaRowset;
class CMySqlSessionPKSchemaRowset;

/////////////////////////////////////////////////////////////////////////////
// CMySqlSession

class ATL_NO_VTABLE CMySqlSession : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IGetDataSourceImpl<CMySqlSession>,
	public IOpenRowsetImpl<CMySqlSession>,
	public ISessionPropertiesImpl<CMySqlSession>,
	public IObjectWithSiteSessionImpl<CMySqlSession>,
	public IDBSchemaRowsetImpl<CMySqlSession>,
	public IDBCreateCommandImpl<CMySqlSession, CProvCommand>,
	public IConnectionSource,
	public ISupportErrorInfoImpl<&IID_IDBSchemaRowset>
{
public:
	CMySqlSession()
	{
	}
	HRESULT FinalConstruct()
	{
		return FInit();
	}
	STDMETHOD(OpenRowset)(IUnknown *pUnk, DBID *pTID, DBID *pInID, REFIID riid,
					   ULONG cSets, DBPROPSET rgSets[], IUnknown **ppRowset)
	{
		CForwardRowset* pRowset;
		return CreateRowset(pUnk, pTID, pInID, riid, cSets, rgSets, ppRowset, pRowset);
	}
	// IConnectionSource
	STDMETHOD(GetConnection)(void **ppConnection)
	{
		USES_CONVERSION;
		if (!ppConnection)
			return E_INVALIDARG;

		*ppConnection = NULL;
		CComPtr<IConnectionSource> spConnSrc;
		HRESULT hr = GetDataSource(IID_IConnectionSource, (IUnknown **)&spConnSrc);
		if (SUCCEEDED(hr))
			hr = spConnSrc->GetConnection(ppConnection);
		return hr;
	}
	STDMETHOD(ReleaseConnection)(void *pConnection)
	{
		delete (CMySqlConnection *)pConnection;
		return S_OK;
	}

BEGIN_PROPSET_MAP(CMySqlSession)
	BEGIN_PROPERTY_SET(DBPROPSET_SESSION)
		PROPERTY_INFO_ENTRY_EX(SESS_AUTOCOMMITISOLEVELS, VT_I4, DBPROPFLAGS_SESSION | DBPROPFLAGS_READ, 0, 0)
	END_PROPERTY_SET(DBPROPSET_SESSION)
	BEGIN_PROPERTY_SET(DBPROPSET_ROWSET)
		PROPERTY_INFO_ENTRY(IAccessor)
		PROPERTY_INFO_ENTRY(IColumnsInfo)
		PROPERTY_INFO_ENTRY(IConvertType)
		PROPERTY_INFO_ENTRY(IRowset)
		PROPERTY_INFO_ENTRY(IRowsetIdentity)
		PROPERTY_INFO_ENTRY(IRowsetInfo)
		PROPERTY_INFO_ENTRY_EX(CANHOLDROWS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_READ, VARIANT_TRUE, 0)
		PROPERTY_INFO_ENTRY_EX(CANFETCHBACKWARDS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_READ , VARIANT_TRUE, 0)
		PROPERTY_INFO_ENTRY_EX(CANSCROLLBACKWARDS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_READ,  VARIANT_TRUE, 0)
	END_PROPERTY_SET(DBPROPSET_ROWSET)
END_PROPSET_MAP()
BEGIN_COM_MAP(CMySqlSession)
	COM_INTERFACE_ENTRY(IGetDataSource)
	COM_INTERFACE_ENTRY(IOpenRowset)
	COM_INTERFACE_ENTRY(ISessionProperties)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IDBCreateCommand)
	COM_INTERFACE_ENTRY(IDBSchemaRowset)
	COM_INTERFACE_ENTRY(IConnectionSource)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
BEGIN_SCHEMA_MAP(CMySqlSession)
	SCHEMA_ENTRY(DBSCHEMA_PROVIDER_TYPES, CMySqlSessionPTSchemaRowset)
	SCHEMA_ENTRY(DBSCHEMA_CATALOGS, CMySqlSessionCatalogsRowset)
	SCHEMA_ENTRY(DBSCHEMA_TABLES, CMySqlSessionTRSchemaRowset)
	SCHEMA_ENTRY(DBSCHEMA_COLUMNS, CMySqlSessionColSchemaRowset)
	SCHEMA_ENTRY(DBSCHEMA_PRIMARY_KEYS, CMySqlSessionPKSchemaRowset)
END_SCHEMA_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CRestrictionsChecker 

template <class T>
class CSchemaRowsetHelper
{
public:
	BOOL CheckRestriction_WSTR(LPOLESTR szString, const VARIANT *rgRestrictions, UINT nRestriction)
	{
		BOOL bResult = TRUE;
		if (rgRestrictions)
		{
			if (rgRestrictions[nRestriction].vt == VT_BSTR)
			{
				if (szString != NULL)
					bResult = !wcsicmp(szString, rgRestrictions[nRestriction].bstrVal);
				else
					bResult = FALSE;
			}
			else if (rgRestrictions[nRestriction].vt == VT_NULL)
			{
				bResult = szString == NULL;
			}
		}
		return bResult;
	}
	HRESULT GetConnectionSource(CComPtr<IConnectionSource> &rspConnSrc)
	{
		// schema rowset always created by session
		T *pT = (T *)this;
		pT->GetSite(IID_IConnectionSource, (void **)&rspConnSrc);
		return rspConnSrc != NULL ? S_OK : E_FAIL;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionCatalogsRowset - DBSCHEMA_CATALOGS

class CMySqlSessionCatalogsRowset:
	public CRowsetImpl< CMySqlSessionCatalogsRowset, CCATALOGS_Row, CMySqlSession>,
	public CSchemaRowsetHelper<CMySqlSessionColSchemaRowset>
{
public:
	HRESULT Execute(LONG* pcRowsAffected, ULONG, const VARIANT*);
	DBSTATUS GetDBStatus(CSimpleRow*, ATLCOLUMNINFO* pInfo);
};

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionTRSchemaRowset - DBSCHEMA_TABLES 

class CMySqlSessionTRSchemaRowset: 
	public CRowsetImpl<CMySqlSessionTRSchemaRowset, CTABLESRowEx, CMySqlSession>,
	public CSchemaRowsetHelper<CMySqlSessionColSchemaRowset>
{
public:
	HRESULT Execute(LONG* pcRowsAffected, ULONG, const VARIANT*);
	DBSTATUS GetDBStatus(CSimpleRow*, ATLCOLUMNINFO* pInfo);
};

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionColSchemaRowset - DBSCHEMA_COLUMNS

class CMySqlSessionColSchemaRowset:
	public CRowsetImpl<CMySqlSessionColSchemaRowset, CCOLUMNSRowEx, CMySqlSession>,
	public CSchemaRowsetHelper<CMySqlSessionColSchemaRowset>
{
public:
	HRESULT Execute(LONG* pcRowsAffected, ULONG, const VARIANT*);
	DBSTATUS GetDBStatus(CSimpleRow*, ATLCOLUMNINFO* pInfo);
protected:
	BYTE GetTypePrecision(DBTYPE wType);
};	

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionPTSchemaRowset - DBSCHEMA_PROVIDER_TYPES

class CMySqlSessionPTSchemaRowset: 
	public CRowsetImpl<CMySqlSessionPTSchemaRowset, CPROVIDER_TYPERowEx, CMySqlSession>,
	public CSchemaRowsetHelper<CMySqlSessionColSchemaRowset>
{
public:
	HRESULT Execute(LONG *pcRowsAffected, ULONG, const VARIANT *);
	DBSTATUS GetDBStatus(CSimpleRow*, ATLCOLUMNINFO* pInfo);
};

/////////////////////////////////////////////////////////////////////////////
// CMySqlSessionPKSchemaRowset - DBSCHEMA_PRIMARY_KEYS

class CMySqlSessionPKSchemaRowset:
	public CRowsetImpl<CMySqlSessionPKSchemaRowset, CPRIMARY_KEYRow, CMySqlSession>,
	public CSchemaRowsetHelper<CMySqlSessionPKSchemaRowset>
{
public:
	HRESULT Execute(LONG *pcRowsAffected, ULONG, const VARIANT *);
	DBSTATUS GetDBStatus(CSimpleRow *, ATLCOLUMNINFO* pInfo);
};

/////////////////////////////////////////////////////////////////////////////

#endif //__CMySqlSession_H_
