#ifndef __ATLDBLINK_H__
#define __ATLDBLINK_H__
#include "myobjects.h"

class CDynamicStorage;
class CDataManager;

///////////////////////////////////////////////////////////////////////////
// CDynamicStorage

class CDynamicStorage
{
public:
	CDynamicStorage();
	CDynamicStorage(const CDynamicStorage &storage);
	CDynamicStorage(CDataManager *pManager, UINT nStorage);
	~CDynamicStorage();
	CDynamicStorage &operator =(const CDynamicStorage &storage);
	// next 2 function is called by ATL OLE DB template framework
	CDynamicStorage *operator &();
	static ATLCOLUMNINFO *GetColumnInfo(void *pThis, ULONG *pcCols);
	// 
	static BOOL LinkToRowset(void *pRowset, CDataManager *pManager);
	static BOOL UnlinkFromRowset(void *pRowset);
	// forwarded from IRowsetImpl::GetDBStatus
	DBSTATUS GetDBStatus(ULONG iOrdinal);

	// control operator &() behavior
	static void RealAddress(BOOL bRealAddress);
	
protected:
	UINT m_nStorage;
	CDataManager *m_pManager;
	static BOOL m_bRealAddress;
	static CComAutoCriticalSection m_section;
	static CSimpleMap<void *, CDataManager *> m_rgRowsetMap;
};

///////////////////////////////////////////////////////////////////////////
// CDataManager

class CDataManager
{
public:
	CDataManager();
	~CDataManager();
	CDynamicStorage *CreateStorage(CMySqlResult *pResult);
	BOOL SetColumnInfo(CMySqlResult *pResult);

protected:
	// next 5 func. is called by CDynamicStorage
	void AddStorageDataRef(UINT nStorageNum);
	void ReleaseStorageData(UINT nStorageNum);
	void *GetStorageData(UINT nStorageNum);
	ATLCOLUMNINFO *GetColumnInfo(BOOL bBookMarks, ULONG *pcCols);
	DBSTATUS GetStorageStatus(UINT nStorage, ULONG iOrdinal);

	// helpers
	BYTE GetTypePrecision(DBTYPE wType);
	BOOL GetDataFromResult(CMySqlResult *pResult, BYTE *pData, DBSTATUS *pStatus);

	// data
	struct DATADESC
	{
		UINT cRef;
		BYTE *pData;
		DBSTATUS *pStatus;
	};
	CSimpleMap<UINT, DATADESC *> m_rgStorageData;
	UINT m_cbData; // суммарная длина данных колонки

	ATLCOLUMNINFO *m_pInfo;
	OLECHAR *m_pStringsBuffer;
	UINT m_cCols;
	UINT m_uNextStorageNum;
	
	// friend class that call protected functions
	friend class CDynamicStorage;
};

///////////////////////////////////////////////////////////////////////////
// CTearedArray

// ATL OLE DB CRowsetImpl<> template requires GetSize(), operator [] 
// and RemoveAll() to be implemented.
template <class T>
class CTearedArray
{
public:
	CTearedArray(): m_nRealSize(0)
	{
	}
	BOOL Add(T& t)
	{
		BOOL bResult = m_rgMap.Add(m_nRealSize, t);
		if (bResult)
			m_nRealSize++;
		return bResult;
	}
	int GetSize() const
	{
		return m_nRealSize;
	}
	void RemoveAll()
	{
		m_nRealSize = 0;
		m_rgMap.RemoveAll();
	}
	T& operator[] (int nIndex) const
	{
		int nKey = m_rgMap.FindKey(nIndex);
		ATLASSERT(nKey != -1);
		return m_rgMap.m_aVal[nKey];
	}
	BOOL TearAt(int nIndex)
	{
		// CSimpleMap FIX:
		// destructor is not called if Remove() last item.
		if (m_rgMap.GetSize() == 1)
		{
			m_rgMap.RemoveAll();
			return TRUE;
		}
		else
			return m_rgMap.Remove(nIndex);
	}
	BOOL IsTornAt(int nIndex)
	{
		return m_rgMap.FindKey(nIndex) == -1;
	}

protected:
	CSimpleMap<int, T> m_rgMap;
	int m_nRealSize;
};

///////////////////////////////////////////////////////////////////////////
// CConnectionPool

#endif //__ATLDBLINK_H__