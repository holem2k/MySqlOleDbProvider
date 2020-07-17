// cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
using namespace std;

class CComInit
{
public:
	CComInit() {::CoInitialize(NULL); }
	~CComInit() {::CoUninitialize(); }
};

int main(int argc, char* argv[])
{
	USES_CONVERSION;
	CComInit com;

	HRESULT hr;
	CDataSource src;
	CDBPropSet set(DBPROPSET_DBINIT);
	set.AddProperty(DBPROP_INIT_DATASOURCE, TEXT("127.0.0.1"));
	set.AddProperty(DBPROP_INIT_CATALOG, TEXT("dbtest2"));
	set.AddProperty(DBPROP_AUTH_USERID, TEXT("root"));
		
	hr = src.Open(TEXT("MySql.1"), &set, 1);
	ATLASSERT(SUCCEEDED(hr));

	CSession session;
	hr = session.Open(src);
	ATLASSERT(SUCCEEDED(hr));

	CTable<CDynamicAccessor, CRowset> table;
	hr = table.Open(session, TEXT("salesreps"));
	ATLASSERT(SUCCEEDED(hr));

	cout << "Table SALESREPS:" << endl;
	while (table.MoveNext() == S_OK)
	{
		// name
		cout << "Name = '" << (char *)table.GetValue(TEXT("name")) << "', ";
		// age
		int age;
		table.GetValue(TEXT("age"), &age);
		cout << "Age = " << age;
		cout << endl;
	}
	return 0;
}
