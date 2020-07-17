#ifndef __MYOBJECTS_H__
#define __MYOBJECTS_H__

#ifndef _WINSOCK2API_
#error You must include 'Winsock2.h' in 'stdafx.h'
#endif

#ifndef __ATLBASE_H__
#error You must include 'atlbase.h' in 'stdafx.h'
#endif

#ifndef __oledb_h__
#error You must include 'oledb.h' in 'stdafx.h'
#endif


#include "..\..\mysql\include\mysql.h"
#include "myobjcore.h"

class CMySqlConnection;

class CMySqlResult;
	class CMySqlForwardResult;
	class CMySqlStaticResult;

///////////////////////////////////////////////////////////////////////////

class CMySqlConnection
{
public:
	CMySqlConnection();
	~CMySqlConnection();

	BOOL Open(LPCTSTR lpServer, LPCTSTR lpUser, 
		LPCTSTR lpPassword, LPCTSTR lpDatabase);
	BOOL Close();
	MYSQL *GetHandle() { ATLASSERT(m_hConnection); return m_hConnection; };
	char *GetError() 
	{
		return mysql_error(m_hConnection); 
	}
	UINT GetErrorNo()
	{
		return mysql_errno(m_hConnection); 
	}
protected:
	BOOL InternalClose();
	MYSQL *m_hConnection;
};

///////////////////////////////////////////////////////////////////////////

enum RESULT_TYPE {RESULT_STATIC, RESULT_FORWARDONLY};

class CMySqlCommand 
{
public:
	CMySqlCommand() {};
	~CMySqlCommand() {};
	BOOL Execute(LPCTSTR lpQuery, CMySqlConnection *pConnection, 
		RESULT_TYPE eType, LONG *pcRowsAffected, CMySqlResult **ppResult);
};

///////////////////////////////////////////////////////////////////////////
// -- Structures

const DWORD CI_PRIMARY_KEY = 0x01;
const DWORD CI_UNIQUE      = 0x02;
const DWORD CI_NOTNULL     = 0x04;
const DWORD CI_AUTOINC     = 0x08;
const DWORD CI_UNSIGNED    = 0x10;
const DWORD CI_FIXED_LENGTH = 0x20;

struct COLUMNINFO
{
	LPCSTR lpColName;
	UINT iOrdinal;
	DBTYPE wType;
	UINT uColumnSize;
	BYTE bPresion;
	BYTE bScale;
	DWORD dwFlags;
};

///////////////////////////////////////////////////////////////////////////

class CMySqlResult
{
public:
	CMySqlResult();
	virtual ~CMySqlResult();
	BOOL GetColumnInfo(UINT *pcColumns, 
		COLUMNINFO **ppInfo, 
		char **ppStringsBuffer);
	BOOL FetchRow();
	//BOOL EOF(BOOL *pbEof);
	BOOL GetData(UINT uOrdinal,
		DBTYPE wType,
		void *pBuffer,
		UINT cbBuf,
		UINT *pcbData,
		BOOL *pbNull,
		BOOL *pbTruncated);
	unsigned __int64 GetCurPos();
	virtual BOOL MoveAt(unsigned __int64 nCurPos) = 0;
	virtual BOOL RowCount(unsigned __int64 *puRowCount) = 0;

protected:
	void Init(MYSQL *hConnection, MYSQL_RES *hResult)
	{
		m_hConnection = hConnection;
		m_hResult = hResult;
	}
	BOOL InternalGetColumnInfo();

	MYSQL *m_hConnection;
	MYSQL_RES *m_hResult;
	MYSQL_ROW m_hRow;
	ULONG *m_pulFieldLength;

	unsigned __int64 m_nCurPos;

	COLUMNINFO *m_pColumnInfo;
	char *m_pStringsBuffer;
	UINT m_cColumns;
	UINT m_cbStrBuf;

	CMySqlDataConvertor m_Convertor;
	friend class CMySqlCommand;
};

/*
	Comments:
	---------
	BOOL GetData(UINT iOrdinal, DBTYPE wType,
		void *pBuffer, UINT cbBuf, UINT *pcbData);
	[*]  iOrdinal - номер колонки (начиная с 1).
	[*]  wType - тип данных
	[*]  pBuffer - указатель на буфер для данных
	[*]  cbBuf - размер буфера 
	[*]  pcbData - размер  подученных данных;
		 для DBTYPE_BLOB и DBTYPE_STR - длина данных,
		 для других типов - sizeof(тип)
*/

///////////////////////////////////////////////////////////////////////////

class CMySqlStaticResult: public CMySqlResult
{
public:
	CMySqlStaticResult();
	~CMySqlStaticResult();
	virtual BOOL MoveAt(unsigned __int64 nCurPos);
	virtual BOOL RowCount(unsigned __int64 *puRowCount);
};

///////////////////////////////////////////////////////////////////////////

class CMySqlForwardResult: public CMySqlResult
{
public:
	CMySqlForwardResult() {};
	~CMySqlForwardResult();
	virtual BOOL MoveAt(unsigned __int64 uCurPos);
	virtual BOOL RowCount(unsigned __int64 *puRowCount);
};

///////////////////////////////////////////////////////////////////////////

#endif //__MYOBJECTS_H__