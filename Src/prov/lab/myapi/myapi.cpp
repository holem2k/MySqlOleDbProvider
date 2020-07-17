// myapi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "myobjects.h"
//#include "myobjcore.h"
#include "stdio.h"
#include <crtdbg.h>
#include "atldblink.h"

int main(int argc, char* argv[])
{
	{
		//CMySqlDataConvertor c;
		//DECIMAL dec;
		//char s[] = "1234567890123456789012345678923.422342343";
		//DBTYPE type;
		//UINT ts;
		//c.ConvertData(FIELD_TYPE_DECIMAL, FALSE, 10, s,  &type, &ts, &dec);
		
		BOOL res;
		CMySqlConnection conn;
		res = conn.Open("localhost", "", "", "test");
		CMySqlCommand comm;
		CMySqlResult *rst;
		UINT uRowsAffected;
		comm.Execute("select * from test2",
			&conn, RESULT_STATIC, &uRowsAffected, &rst);
		
		//UINT cColumns;
		//COLUMNINFO *pColumnInfo;
		//char *pStrBuf;
		//rst->GetColumnInfo(&cColumns, &pColumnInfo, &pStrBuf);
		
		//	rst->FetchRow();
		//rst->FetchRow();
		//rst->FetchRow();
		//	DBTIMESTAMP id;
		//	BOOL bNull;
		//	UINT cbData;
		//	rst->GetData(2, DBTYPE_DBTIMESTAMP, &id, sizeof(id), &cbData, &bNull);
		CDataManager m;
		ATLASSERT(m.SetColumnInfo(rst));
		CDynamicStorage *s;
		ATLASSERT(rst->FetchRow());
		s = m.CreateStorage(rst);
		
		delete  s;
		delete rst;
	}
	_CrtDumpMemoryLeaks();
	return 0;
}
