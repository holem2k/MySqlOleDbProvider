// MySqlRS.h : Declaration of the CForwardRowset
#ifndef __CForwardRowset_H_
#define __CForwardRowset_H_
#include "resource.h"       // main symbols
#include "lab\myapi\atldblink.h" 
#include "prov.h"
#include "RowsetExImpl.h"

class CForwardRowset;

////////////////////////////////////////////////////////////////////////////
// CMySqlCommand

class ATL_NO_VTABLE CProvCommand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IAccessorImpl<CProvCommand>,
	public ICommandTextImpl<CProvCommand>,
	public ICommandPropertiesImpl<CProvCommand>,
	public IObjectWithSiteImpl<CProvCommand>,
	public IConvertTypeImpl<CProvCommand>,
	public IColumnsInfoImpl<CProvCommand>,
	public CCommandGetColumnInfoSupport<CProvCommand, CForwardRowset, CDynamicStorage>
{
public:
BEGIN_COM_MAP(CProvCommand)
	COM_INTERFACE_ENTRY(ICommand)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IAccessor)
	COM_INTERFACE_ENTRY(ICommandProperties)
	COM_INTERFACE_ENTRY2(ICommandText, ICommand)
	COM_INTERFACE_ENTRY(IColumnsInfo)
	COM_INTERFACE_ENTRY(IConvertType)
END_COM_MAP()
// ICommand
public:
	HRESULT FinalConstruct()
	{
		HRESULT hr = CConvertHelper::FinalConstruct();
		if (FAILED (hr))
			return hr;
		hr = IAccessorImpl<CProvCommand>::FinalConstruct();
		if (FAILED(hr))
			return hr;
		return CUtlProps<CProvCommand>::FInit();
	}
	void FinalRelease()
	{
		IAccessorImpl<CProvCommand>::FinalRelease();
	}
	HRESULT WINAPI Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, 
						  LONG * pcRowsAffected, IUnknown ** ppRowset);

	// IColumnsInfoImpl override
	// Реализация IColumnsInfo::GetColumnInfo не позволяет успешно
	// НЕ возвращать результат
	STDMETHOD(GetColumnInfo)(ULONG *pcColumns,
		DBCOLUMNINFO **prgInfo,
		OLECHAR **ppStringsBuffer);
	static ATLCOLUMNINFO* GetColumnInfo(CProvCommand* pv, ULONG* pcInfo);

BEGIN_PROPSET_MAP(CProvCommand)
	BEGIN_PROPERTY_SET(DBPROPSET_ROWSET)
		PROPERTY_INFO_ENTRY(IAccessor)
		PROPERTY_INFO_ENTRY(IColumnsInfo)
		PROPERTY_INFO_ENTRY(IConvertType)
		PROPERTY_INFO_ENTRY(IRowset)
		PROPERTY_INFO_ENTRY(IRowsetIdentity)
		PROPERTY_INFO_ENTRY(IRowsetInfo)
		PROPERTY_INFO_ENTRY_EX(IRowsetLocate, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_WRITE | DBPROPFLAGS_READ | DBPROPFLAGS_CHANGE, VARIANT_FALSE, 0)
		PROPERTY_INFO_ENTRY_EX(BOOKMARKS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_WRITE | DBPROPFLAGS_READ | DBPROPFLAGS_CHANGE, VARIANT_FALSE, 0)
		PROPERTY_INFO_ENTRY_EX(LITERALBOOKMARKS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_WRITE | DBPROPFLAGS_READ | DBPROPFLAGS_CHANGE, VARIANT_FALSE, 0)
		PROPERTY_INFO_ENTRY_EX(ORDEREDBOOKMARKS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_WRITE | DBPROPFLAGS_READ | DBPROPFLAGS_CHANGE, VARIANT_FALSE, 0)
		PROPERTY_INFO_ENTRY(BOOKMARKSKIPPED)
		PROPERTY_INFO_ENTRY_EX(BOOKMARKTYPE,  VT_I4, DBPROPFLAGS_ROWSET | DBPROPFLAGS_READ, DBPROPVAL_BMK_NUMERIC, 0)
		PROPERTY_INFO_ENTRY_EX(CANFETCHBACKWARDS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_WRITE | DBPROPFLAGS_READ | DBPROPFLAGS_CHANGE, VARIANT_FALSE, 0)
		PROPERTY_INFO_ENTRY_EX(CANSCROLLBACKWARDS, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_WRITE | DBPROPFLAGS_READ | DBPROPFLAGS_CHANGE, VARIANT_FALSE, 0)
		PROPERTY_INFO_ENTRY_EX(QUICKRESTART, VT_BOOL, DBPROPFLAGS_ROWSET | DBPROPFLAGS_WRITE | DBPROPFLAGS_READ | DBPROPFLAGS_CHANGE, VARIANT_FALSE, 0)
		PROPERTY_INFO_ENTRY(CANHOLDROWS)
	END_PROPERTY_SET(DBPROPSET_ROWSET)
END_PROPSET_MAP()

protected:
	// Определение генерирует ли команда набор рядов
	// S_OK - да, S_FALSE - нет
	static HRESULT IsCommandGenerateRows(LPCOLESTR szCommandText);
};

////////////////////////////////////////////////////////////////////////////
// CForwardRowset

class CForwardRowset: public CRowsetImpl< CForwardRowset, CDynamicStorage, CProvCommand, CTearedArray<CDynamicStorage> >,
					   public IIsRowset,
					   public ISupportErrorInfoImpl<&IID_IRowset>,
					   public CIOpenRowsetSupport<CForwardRowset>
{
public:
	CForwardRowset();
	HRESULT Execute(DBPARAMS * pParams, LONG* pcRowsAffected);
	HRESULT GetCommandFromID(DBID* pTableID, DBID* pIndexID);
	HRESULT ValidateCommandID(DBID* pTableID, DBID* pIndexID);
	OUT_OF_LINE HRESULT GetDataHelper(HACCESSOR hAccessor,
		ATLCOLUMNINFO *&rpInfo,
		void **ppBinding,
		void *&rpSrcData,
		ULONG &rcCols,
		CComPtr<IDataConvert> &rspConvert,
		_HRowClass *pRow);
	// IRowset
	STDMETHOD(GetNextRows)(HCHAPTER /*hReserved*/,
		LONG lRowsOffset,
		LONG cRows,
		ULONG *pcRowsObtained,
		HROW **prghRows);
	STDMETHOD(ReleaseRows)(ULONG cRows,
		const HROW rghRows[],
		DBROWOPTIONS rgRowOptions[],
		ULONG rgRefCounts[],
		DBROWSTATUS rgRowStatus[]);
	STDMETHOD(RestartPosition)(HCHAPTER /*hReserved*/);
	
	
	// IIsRowset
	STDMETHOD(IsRowset)()
	{
		return m_bIsRowset ? S_OK : S_FALSE;
	}

	DBSTATUS GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pInfo);
	void SetRuntimeProperties();
	void FinalRelease();

	typedef CRowsetImpl< CForwardRowset, CDynamicStorage, CProvCommand, CTearedArray<CDynamicStorage> > _ForwardRowsetBaseClass;
	BEGIN_COM_MAP(CForwardRowset)
		 COM_INTERFACE_ENTRY(IIsRowset)
		 COM_INTERFACE_ENTRY(ISupportErrorInfo)
		 COM_INTERFACE_ENTRY_CHAIN(_ForwardRowsetBaseClass)
	END_COM_MAP()

protected:
	// helpers
	HRESULT GetConnectionSource(IConnectionSource **pConnSource);
	BOOL InitializeRowset(LONG *pcRowsAffected);
	BOOL UninitializeRowset();
	BOOL FetchNextRows(UINT cRows);

	CDataManager *m_pManager;
	CMySqlResult *m_pResult;
	CMySqlConnection *m_pConnection;
	BOOL m_bIsRowset;
};

////////////////////////////////////////////////////////////////////////////
// CStaticRowset

class CStaticRowset: public CRowsetImpl<CStaticRowset, CDynamicStorage,
                            CProvCommand, CSimpleArray<CDynamicStorage>,
							CSimpleRow,
							IRowsetLocateImpl<CStaticRowset, IRowsetLocate, UINT> >,
					 public IIsRowset,
					 public CIOpenRowsetSupport<CStaticRowset>
{
public:
	CStaticRowset();
	HRESULT Execute(DBPARAMS * pParams, LONG* pcRowsAffected);
	HRESULT GetCommandFromID(DBID* pTableID, DBID* pIndexID);
	HRESULT ValidateCommandID(DBID* pTableID, DBID* pIndexID);
	OUT_OF_LINE HRESULT GetDataHelper(HACCESSOR hAccessor,
		ATLCOLUMNINFO *&rpInfo,
		void **ppBinding,
		void *&rpSrcData,
		ULONG &rcCols,
		CComPtr<IDataConvert> &rspConvert,
		_HRowClass *pRow);

	// IIsRowset
	STDMETHOD(IsRowset)()
	{
		return m_bIsRowset ? S_OK : S_FALSE;
	}

	// IObjectWithSite (override method from IRowsetCreatorImpl)
	STDMETHOD(SetSite)(IUnknown* pCreator);

	DBSTATUS GetDBStatus(CSimpleRow *pRow, ATLCOLUMNINFO *pInfo);
	void FinalRelease();
	void SetRuntimeProperties();

    typedef CRowsetImpl<CStaticRowset, CDynamicStorage, CProvCommand, CSimpleArray<CDynamicStorage>, CSimpleRow,IRowsetLocateImpl<CStaticRowset, IRowsetLocate, UINT> >  _StaticRowsetBaseClass;
	BEGIN_COM_MAP(CStaticRowset)
		COM_INTERFACE_ENTRY(IRowsetLocate)
		COM_INTERFACE_ENTRY(IIsRowset)
		COM_INTERFACE_ENTRY_CHAIN(_StaticRowsetBaseClass)
	END_COM_MAP()

protected:
	// helpers
	HRESULT GetConnectionSource(IConnectionSource **pConnSource);
	BOOL InitializeRowset(LONG *pcRowsAffected);
	BOOL UninitializeRowset();
	BOOL FetchAllRows();

	CDataManager *m_pManager;
	CMySqlResult *m_pResult;
	CMySqlConnection *m_pConnection;
	BOOL m_bIsRowset;
};

////////////////////////////////////////////////////////////////////////////

#endif //__CForwardRowset_H_
