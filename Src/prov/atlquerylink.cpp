#include "stdafx.h"
#include "atlquerylink.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////
// CSimpleQuery

CSimpleQuery::CSimpleQuery(REFGUID rguidError)
{
	m_guidError = rguidError;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CSimpleQuery::Execute(CMySqlConnection *pConnection, LPCTSTR lpQuery, 
		VARIANT *pvValue)
{
	USES_CONVERSION;

	BOOL bResult = TRUE;
	CMySqlCommand command;
	CMySqlResult *pResult;
	if (command.Execute(lpQuery, pConnection, RESULT_STATIC, NULL, &pResult))
	{
		if (pResult && pResult->FetchRow())
		{
			if (InternalGetData(pResult, pvValue))
				bResult = TRUE;
		}
		delete pResult;
	}
	else 
		CreateError(ERRORSRC, A2OLE(pConnection->GetError()), m_guidError);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CSimpleQuery::InternalGetData(CMySqlResult *pResult, VARIANT *pvValue)
{
	USES_CONVERSION;
	BOOL bTruncated, bNull;
	UINT cData;

	UINT uData;
	if (pResult->GetData(1, DBTYPE_I4, &uData, sizeof(uData), &cData, &bNull, &bTruncated))
	{
		if (!bNull)
		{
			VariantClear(pvValue);
			pvValue->vt = VT_I4;
			pvValue->lVal = uData;
			return TRUE;
		}
	}

	char buffer[16384];
	if (pResult->GetData(1, DBTYPE_STR, buffer, 16384, &cData, &bNull, &bTruncated))
	{
		if (bNull)
		{
			VariantClear(pvValue);
			pvValue->vt = VT_BSTR;
			pvValue->bstrVal = ::SysAllocString(L"");
			return TRUE;
		}
		if (!bTruncated)
		{
			VariantClear(pvValue);
			pvValue->vt = VT_BSTR;
			pvValue->bstrVal = ::SysAllocString(A2OLE(buffer));
			return TRUE;
		}

	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
