#include "stdafx.h"
#include "RowsetExImpl.h"

///////////////////////////////////////////////////////////////////////////
// IDataConvertImplProxy

IDataConvertImplProxy::IDataConvertImplProxy(IDataConvert *pConvert)
{
	ATLASSERT(pConvert);
	m_pConvert = pConvert;
	m_pConvert->AddRef();
	m_cProxyRef = 0;
}

///////////////////////////////////////////////////////////////////////////

STDMETHODIMP IDataConvertImplProxy::QueryInterface(REFIID riid, LPVOID *ppv)
{
	return m_pConvert->QueryInterface(riid, ppv);
}

///////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(ULONG) IDataConvertImplProxy::AddRef()
{
	m_cProxyRef++;
	return  m_pConvert->AddRef();
}

///////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(ULONG) IDataConvertImplProxy::Release()
{
	IDataConvert *pConvert = m_pConvert;
	if (--m_cProxyRef == 0)
		delete this;
	return pConvert->Release();
}

///////////////////////////////////////////////////////////////////////////

STDMETHODIMP IDataConvertImplProxy::CanConvert(DBTYPE wSrcType,
											   DBTYPE wDstType)
{
	return m_pConvert->CanConvert(wSrcType, wDstType);
}

///////////////////////////////////////////////////////////////////////////

STDMETHODIMP IDataConvertImplProxy::DataConvert (DBTYPE wSrcType,
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
												 DBDATACONVERT dwFlags)
{
	ULONG cbSrcNewLength = cbSrcLength;
	void *pNewSrc = pSrc;
	if (wSrcType == DBTYPE_BYTES)
	{
		cbSrcNewLength = *(ULONG *)pSrc;
		pNewSrc = (ULONG *)pSrc + 1;
	}
	return m_pConvert->DataConvert(wSrcType,
		wDstType,
		cbSrcNewLength,
		pcbDstLength,
		pNewSrc,
		pDst,
		cbDstMaxLength,
		dbsSrcStatus,
		pdbsStatus,
		bPrecision,
		bScale,
		dwFlags);
}

///////////////////////////////////////////////////////////////////////////

STDMETHODIMP IDataConvertImplProxy::GetConversionSize(DBTYPE wSrcType,
													  DBTYPE wDstType,
													  ULONG *pcbSrcLength,
													  ULONG *pcbDstLength,
													  void *pSrc)
{
	return m_pConvert->GetConversionSize(wSrcType,
		wDstType,
		pcbSrcLength,
		pcbDstLength,
		pSrc);
}

///////////////////////////////////////////////////////////////////////////