#include "stdafx.h"
#include "error.h"

///////////////////////////////////////////////////////////////////////

HRESULT CreateError(LPCOLESTR szErrorSrc, LPCOLESTR szDescription, REFGUID rguid)
{
	CComPtr<ICreateErrorInfo> spICEI;
	if (SUCCEEDED(CreateErrorInfo(&spICEI)))
	{
		spICEI->SetSource((LPOLESTR)szErrorSrc);
		spICEI->SetDescription((LPOLESTR)szDescription);
		spICEI->SetGUID(rguid);
		CComPtr<IErrorInfo> spErrorInfo;
		if (SUCCEEDED(spICEI->QueryInterface(IID_IErrorInfo, (void **)&spErrorInfo)))
			SetErrorInfo(0, spErrorInfo);
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////

