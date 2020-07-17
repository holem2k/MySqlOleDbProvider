#ifndef __ROWSETEXIMPL_H__
#define __ROWSETEXIMPL_H__

///////////////////////////////////////////////////////////////////////////
// IRowsetLocate implementation

template <class T, class RowsetInterface, class BookmarkType>
class ATL_NO_VTABLE IRowsetLocateImpl: public IRowsetImpl<T, RowsetInterface>
{
public:
	IRowsetLocateImpl()
	{
		m_bOrderedBookmarks = false;
	}

	STDMETHOD(Compare)(HCHAPTER hChapter,
		DBBKMARK cbBookmark1,
		const BYTE *pBookmark1,
		DBBKMARK cbBookmark2,
		const BYTE *pBookmark2,
		DBCOMPARE *pComparison)
	{
		ATLTRACE2(atlTraceDBProvider, 0, "IRowsetLocateImpl::Compare\n");
		if (!pComparison)
			return E_INVALIDARG;
		
		HRESULT hr;
		if (FAILED(hr = ValidateBookmark(pBookmark1, cbBookmark1)))
			return hr;

		if (FAILED(hr = ValidateBookmark(pBookmark2, cbBookmark2)))
			return hr;
		
		*pComparison = CompareBookmarks(pBookmark1, cbBookmark1, pBookmark2, cbBookmark2);
		return S_OK;
	}
	STDMETHOD(GetRowsAt)(HWATCHREGION   hReserved1,
		HCHAPTER hChapter,
		DBBKMARK cbBookmark,
		const BYTE *pBookmark,
		DBROWOFFSET lRowsOffset,
		DBROWCOUNT cRows,
		DBCOUNTITEM *pcRowsObtained,
		HROW **prghRows)
	{
		ATLTRACE2(atlTraceDBProvider, 0, "IRowsetLocateImpl::GetRowsAt\n");
		if (!prghRows || !pcRowsObtained)
			return E_INVALIDARG;

		HRESULT hr = ValidateBookmark(pBookmark, cbBookmark);
		if (FAILED(hr))
			return hr;

		T *pT = (T *)this;

		UINT iRowset;
		BOOL bFind = FALSE;
		if (cbBookmark == STD_BOOKMARKLENGTH)
		{
			bFind = TRUE;
			if (*pBookmark == DBBMK_FIRST)
				iRowset = 0;
			else if (*pBookmark == DBBMK_LAST)
				iRowset = pT->m_rgRowData.GetSize() - 1;
			else
				bFind = FALSE;
		}
		if (!bFind)
		{
			for (int i = 0; i < pT->m_rgRowData.GetSize(); i++)
			{
				// get pointer to current row bookmark
				BookmarkType *pCurBmk = (BookmarkType *)&pT->m_rgRowData[i];
				if (*((BookmarkType *)pBookmark) == *pCurBmk)
				{
					iRowset = i;
					bFind = TRUE;
					break;
				}
			}
		}
		ATLASSERT(bFind);

		UINT iRowsetTemp = pT->m_iRowset;
		pT->m_iRowset = iRowset;
		hr = GetNextRows(hChapter, lRowsOffset, cRows, pcRowsObtained, prghRows);
		pT->m_iRowset = iRowsetTemp;

		return hr;
	}
	STDMETHOD(GetRowsByBookmark)(HCHAPTER hChapter,
		DBCOUNTITEM cRows,
		const DBBKMARK rgcbBookmarks[],
		const BYTE  *rgpBookmarks[],
		HROW rghRows[],
		DBROWSTATUS rgRowStatus[])
	{
		ATLTRACE2(atlTraceDBProvider, 0, "IRowsetLocateImpl::GetRowsByBookmark\n");
		if (!rghRows || !rgcbBookmarks || !rgpBookmarks)
			return E_INVALIDARG;
		if (cRows == 0)
			return S_OK;

		T *pT = (T *)this;
		// assume ok
		HRESULT hr = S_OK;
		for (UINT i = 0; i < cRows; i++)
		{
			HRESULT hrRow;
			HRESULT hrBmk = ValidateBookmark(rgpBookmarks[i], rgcbBookmarks[i]);
			if (SUCCEEDED(hrBmk))
			{
				// if standard bookmark
				if (rgcbBookmarks[i] == STD_BOOKMARKLENGTH &&
					(*rgpBookmarks[i] == DBBMK_FIRST ||
					*rgpBookmarks[i] == DBBMK_LAST))
					hrBmk = E_FAIL;
				if (SUCCEEDED(hrBmk))
				{
					UINT iRowset;
					if (GetRowIndexByBookmark((BookmarkType *)rgpBookmarks[i], &iRowset) == S_OK)
					{
						ULONG cRowsObtained = i;
						hrRow = CreateRow(iRowset, cRowsObtained, rghRows);
						if (SUCCEEDED(hrRow))
						{
							if (rgRowStatus)
								rgRowStatus[i] = DBROWSTATUS_S_OK;
						}
					}
				}
			}
			if (FAILED(hrBmk))
			{
				rghRows[i] = DB_NULL_HROW;
				if (rgRowStatus)
					rgRowStatus[i] =  hrRow == E_OUTOFMEMORY ? DBROWSTATUS_E_OUTOFMEMORY  
						: DBROWSTATUS_E_INVALID;
				hr = DB_S_ERRORSOCCURRED;
			}
		}
		return hr;
	}
	STDMETHOD(Hash)(HCHAPTER hChapter,
		DBBKMARK cBookmarks,
		const DBBKMARK rgcbBookmarks[],
		const BYTE *rgpBookmarks[],
		DBHASHVALUE rgHashedValues[],
		DBROWSTATUS rgBookmarkStatus[])
	{
		ATLTRACE2(atlTraceDBProvider, 0, "IRowsetLocateImpl::Hash\n");
		return E_FAIL;
	}

protected:
	// pBookmark1 and pBookmark2 must be VALID !
	DBCOMPARE CompareBookmarks(const BYTE *pBookmark1, UINT cbBookmark1, 
		const BYTE *pBookmark2, UINT cbBookmark2)
	{
		T *pT = (T *)this;
		BookmarkType bmk1, bmk2;

		if (cbBookmark1 == STD_BOOKMARKLENGTH)
			bmk1 = (BookmarkType)*pBookmark1;
		else
			bmk1 = *((BookmarkType *)pBookmark1);
		if (cbBookmark2 == STD_BOOKMARKLENGTH)
			bmk2 = (BookmarkType)*pBookmark2;
		else
			bmk2 = *((BookmarkType *)pBookmark2);

		if (!m_bOrderedBookmarks)
			return bmk1 == bmk2 ? DBCOMPARE_EQ : DBCOMPARE_NE;

		if (bmk1 == DBBMK_FIRST)
		{
			DBCOMPARE dbComp;
			if (bmk2 == DBBMK_FIRST)
				dbComp = DBCOMPARE_EQ;
			else 
				dbComp = DBCOMPARE_NE;
			return dbComp;
		}
		else if (bmk1 == DBBMK_LAST)
		{
			DBCOMPARE dbComp;
			if (bmk2 == DBBMK_LAST)
				dbComp = DBCOMPARE_EQ;
			else 
				dbComp = DBCOMPARE_NE;
			return dbComp;
		}

		DBCOMPARE dbComp = DBCOMPARE_EQ;
		if (bmk1 < bmk2)
			dbComp = DBCOMPARE_LT;
		else if (bmk1 > bmk2)
			dbComp = DBCOMPARE_GT;
		return dbComp;
	}
	HRESULT ValidateBookmark(const BYTE *pBookmark, ULONG cbBookmark)
	{
		if (!pBookmark || cbBookmark == 0)
			return E_INVALIDARG;

		// if standard bookmark
		if (cbBookmark == STD_BOOKMARKLENGTH)
		{
			if (*pBookmark == DBBMK_FIRST || *pBookmark == DBBMK_LAST)
				return S_OK;
			if (*pBookmark == DBBMK_INVALID)
				return DB_E_BADBOOKMARK;
		}

		if (cbBookmark != sizeof(BookmarkType))
			return DB_E_BADBOOKMARK;

		BookmarkType *pBmk = (BookmarkType *)pBookmark;
		T *pT = (T *)this;

		HRESULT hr = DB_E_BADBOOKMARK;
		for (int i = 0; i < pT->m_rgRowData.GetSize(); i++)
		{
			if (*pBmk == *((BookmarkType *)&pT->m_rgRowData[i]))
			{
				hr = S_OK;
				break;
			}
		}
		return hr;
	}
	// Возвращает индекс ряда в m_rgRowData имеющего 
	// заданную закладку. Закладка не может быть стандартной.
	// S_OK - закладка найдена, S_FALSE - закладка не найдена
	HRESULT GetRowIndexByBookmark(BookmarkType *pBookmark, UINT *pIndex)
	{
		ATLASSERT(pIndex);
	
		HRESULT hr = S_FALSE;
		T *pT = (T *)this;
		for (int i = 0; i < pT->m_rgRowData.GetSize(); i++)
		{
			if (*pBookmark == *((BookmarkType *)&pT->m_rgRowData[i]))
			{
				*pIndex = i;
				break;
			}
		}
		return hr;
	}
	unsigned m_bOrderedBookmarks:1;
};

///////////////////////////////////////////////////////////////////////////
// CIOpenRowsetSupport
//| Наследуется объектом-командой, который должен переопределить
//| GetCommandFromID и ValidateCommandID и в реализации 
//| этих методов вызывать соответствующие Helper'ы.

template <class T>
class CIOpenRowsetSupport
{
public:
	HRESULT GetCommandFromIDHelper(DBID* pTableID, DBID* pIndexID)
	{
		T *pT = (T *)this;
		HRESULT hr = S_OK;
		pT->m_strIndexText.Empty();

		CComBSTR bstr(OLESTR("SELECT * FROM "));
		bstr.Append(pTableID->uName.pwszName);
		pT->m_strCommandText = bstr;
		if (pT->m_strCommandText == (BSTR)NULL)
			hr = E_OUTOFMEMORY;
		return hr;

	};
	HRESULT ValidateCommandIDHelper(DBID* pTableID, DBID* pIndexID)
	{
		HRESULT hr = S_OK;
		if (pIndexID)
			hr = DB_E_NOINDEX;
		return hr;
	}
};

///////////////////////////////////////////////////////////////////////////
// CCommandGetColumnInfoSupport
//| Наследуется объектом-командой, 
//| GetColumnInfoHelper вызывается в ATLCOLUMNINFO* GetColumnInfo(...),
//| который должен возвратить m_pInfo.

template<class T, class Rowset, class Storage>
class CCommandGetColumnInfoSupport
{
public:
	CCommandGetColumnInfoSupport()
	{
		m_pInfo = NULL;
		m_pStringBuf = NULL;
		m_cCols = 0;
	}
	~CCommandGetColumnInfoSupport()
	{
		InternalClose();
	}
	HRESULT GetColumnInfoHelper()
	{
		T *pT = (T *)this;

		InternalClose();
		IUnknown *pUnk;
		Rowset *pRowset;
		HRESULT hr = pT->CreateRowset(NULL, IID_IRowset, NULL, NULL, &pUnk, pRowset);
		if (FAILED(hr))
			return hr;

		ATLCOLUMNINFO *pInfo;
		ULONG cCols;
		pInfo = Storage::GetColumnInfo(pRowset, &cCols);
		if (pInfo)
		{
			ULONG cbStrBuf = 0;
			for (ULONG i = 0; i < cCols; i++)
				cbStrBuf += lstrlenW(pInfo[i].pwszName) + 1;
			cbStrBuf *= 2;

			m_pInfo = new ATLCOLUMNINFO[cCols];
			m_pStringBuf = new OLECHAR[cbStrBuf];
			if (m_pInfo && m_pStringBuf)
			{
				ULONG uStrStart = 0;
				for (ULONG i = 0; i < cCols; i++)
				{
					m_pInfo[i] = pInfo[i];
					lstrcpyW(m_pStringBuf + uStrStart, pInfo[i].pwszName);
					m_pInfo[i].pwszName = m_pStringBuf + uStrStart;
					if (m_pInfo[i].columnid.eKind == DBKIND_NAME)
						m_pInfo[i].columnid.uName.pwszName = m_pStringBuf + uStrStart;
					uStrStart += lstrlenW(pInfo[i].pwszName) + 1;
				}
				m_cCols = cCols;
			}
			else
				InternalClose();
		}
		pUnk->Release();
		hr = m_pInfo ? S_OK : E_FAIL;
		return hr;
	}
protected:
	void InternalClose()
	{
		if (m_pInfo)
			delete [] m_pInfo;
		if (m_pStringBuf)
			delete [] m_pStringBuf;
		m_pInfo = NULL;
		m_cCols = 0;
	}
	ATLCOLUMNINFO *m_pInfo;
	OLECHAR *m_pStringBuf;
	ULONG m_cCols;
};

///////////////////////////////////////////////////////////////////////////
// IDataConvertImplProxy
//| Proxy для стандартного объекта, реализующего IDataConvert;
//| перехватывает попытки конвертировать данные типа DBTYPE_BYTES,
//| устанавливая правильное значение длины данных 
//| (реализация ATL IRowset::GetData(...) при вызове 
//| IDataConvert::ConvertData(DBTYPE_BYTES, ...) в качестве длины данных
//| передает размер колонки.)
//| Реальная длина данных должна хранится в первых 4х байтах данных,
//| за которыми сразу следуют данные DBTYPE_BYTES.
//| Время жизни прокси
//| ------------------
//| При запросе объекта, реализующего IDataConvert, возвращается
//| указатель на прокси. 

class IDataConvertImplProxy: public IDataConvert
{
public:
	IDataConvertImplProxy(IDataConvert *pConvert);
	STDMETHOD(QueryInterface)(REFIID, LPVOID *);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(CanConvert)(DBTYPE wSrcType,
		DBTYPE   wDstType);
	STDMETHOD(DataConvert)(DBTYPE wSrcType,
		DBTYPE wDstType,
		ULONG cbSrcLength,
		ULONG *pcbDstLength,
		void *pSrc,
		void *pDst,
		ULONG cbDstMaxLength,
		DBSTATUS dbsSrcStatus,
		DBSTATUS *pdbsStatus,
		BYTE bPrecision,
		BYTE bScale,
		DBDATACONVERT dwFlags);
	STDMETHOD(GetConversionSize)(DBTYPE	wSrcType,
		DBTYPE	wDstType,
		ULONG *pcbSrcLength,
		ULONG *pcbDstLength,
		void *pSrc);

protected:
	IDataConvert *m_pConvert;
	ULONG m_cProxyRef;
};

///////////////////////////////////////////////////////////////////////////

#endif //__ROWSETEXIMPL_H__