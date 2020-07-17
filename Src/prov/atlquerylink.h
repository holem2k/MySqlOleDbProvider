#ifndef __ATLQUERYLINK_H__
#define __ATLQUERYLINK_H__
#include "lab\myapi\myobjects.h"

/////////////////////////////////////////////////////////////////////////////
// CSimpleQuery

class CSimpleQuery
{
public:
	CSimpleQuery(REFGUID rguidError);
	BOOL Execute(CMySqlConnection *pConnection, LPCTSTR lpQuery, 
		VARIANT *pvValue);
protected:
	BOOL InternalGetData(CMySqlResult *pResult, VARIANT *pvValue);
	GUID m_guidError;
};

#endif //__ATLQUERYLINK_H__